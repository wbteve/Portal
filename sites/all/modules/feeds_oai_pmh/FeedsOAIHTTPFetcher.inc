<?php


/**
 * Definition of the import batch object created on the fetching stage by
 * FeedsOAIHTTPFetcher.
 */
class FeedsOAIHTTPBatch extends FeedsImportBatch {
  protected $oai_endpoint_url;
  protected $from_timestamp;
  protected $until_timestamp;
  protected $granularity;

  /**
   * Constructor.
   */
  public function __construct($oai_endpoint_url, $from_timestamp, $until_timestamp, $granularity, $resumption_token, $set) {
    $this->oai_endpoint_url = $oai_endpoint_url;
    $this->from_timestamp = $from_timestamp;
    $this->until_timestamp = $until_timestamp;
    $this->granularity = $granularity;
    $this->set = $set;
    $this->resumption_token = $resumption_token;
    parent::__construct();
  }
  
  /**
   * Implementation of FeedsImportBatch::getRaw();
   */
  public function getRaw() {
    // TODO: Move the URL building and data fetching to feeds_oai_pmh.inc
    // Build the request URL
    $url = $this->oai_endpoint_url;
    $url .= '?verb=ListRecords';
    if ($this->resumption_token) {
      $url .= "&resumptionToken=" . rawurldecode($this->resumption_token);
    }
    else {
      // When a resumptionToken is issued, there can't be any other arguments
      // in the request.
      $url .= '&metadataPrefix=oai_dc';
      if ($this->from_timestamp > 0) {
        $url .= '&from=' . rawurlencode($this->formatDate($this->from_timestamp));
      }
      if ($this->until_timestamp > 0) {
          $url .= '&until=' . rawurlencode($this->formatDate($this->until_timestamp));
        }
      if ($this->set) {
        $url .= '&set=' . rawurlencode($this->set);
      }
    }
    
    #dsm("URL for OAI request: $url");

    // Fetch the feed's contents
    $result = drupal_http_request($url);

    if ($result->code == 200) {
      // TODO: Use simpleXML instead of regexp
      // Try to get resumptionToken. Example:
      // <resumptionToken completeListSize="478" cursor="0">0/300/478/oai_dc/eserev/null/null</resumptionToken>
      $ok = preg_match_all('/<resumptionToken.*?>([^<]+)<\/resumptionToken>/s', $result->data, $matches);
      if ($ok) {
        $resumption_token = array_pop($matches[1]);
        #dsm("Resumption token: $resumption_token");
        $this->setLastDate(0);
      }
      else {
        // No resumption token in response. Store current system timestamp
        // so next request limits items returned.
        $resumption_token = "";
        $this->setLastDate(time());
      }
      $this->setResumptionToken($resumption_token);
    }
    else {
      // OAI fetch failed
      #dsm("OAI fetch failed with error : " . $result->error);
      // TODO: Log via watchdog?
    }
    // Return the feed's contents
    return $result->data;
  }

  protected function setResumptionToken($resumption_token) {
    $this->resumption_token = $resumption_token;
    variable_set('feeds_oai:resumptionToken:' . $this->set . ':' . $this->oai_endpoint_url, $resumption_token);
  }

  protected function setLastDate($timestamp) {
    variable_set('feeds_oai:from:' . $this->set . ':' . $this->oai_endpoint_url, $timestamp);
  }

  protected function formatDate($timestamp) {
    $granularity = $this->granularity;
    if ('seconds' == $granularity) {
      $date_format = 'Y-m-d\TH:m:s\Z';
    }
    elseif ('days' == $granularity) {
      $date_format = 'Y-m-d';
    }
    return date($date_format, $timestamp);
  }
}

 
/**
 * Fetcher class for OAI-PMH repository webservices.
 */
class FeedsOAIHTTPFetcher extends FeedsHTTPFetcher {
  /**
   * Fetch content from feed.
   */
  public function fetch(FeedsSource $source) {
    global $conf;
    
    $source_config = $source->getConfigFor($this);

    $from_timestamp = FALSE;
    $until_timestamp = FALSE;
    
    $resumption_token = variable_get('feeds_oai:resumptionToken:' . $source_config['set'] . ':' . $source_config['source'], '');
    if (!$resumption_token) {
      $from_timestamp = (int)variable_get('feeds_oai:from:' . $source_config['set'] . ':' . $source_config['source'], FALSE);
      #dsm("from_timestamp = $from_timestamp");
      if ($from_timestamp > 0) {
        $from_timestamp = $from_timestamp + 1;
        // Set ending date for query to 60 days from last-fetched document
        #$until_timestamp = $from_timestamp + (60 * 24 * 3600);
      }
    }

    // The setSpec to harvest from.
    $set = $source_config['set'];

    return new FeedsOAIHTTPBatch(
      $source_config['source'],
      $from_timestamp,
      $until_timestamp,
      $source_config['granularity'],
      $resumption_token,
      $set
    );
  }

  /**
   * Declare defaults.
   */
  public function configDefaults() {
    // TODO: is this needed?
    return array(
      'last_fetched_timestamp' => '',
      'earliest_timestamp' => '',
      'granularity' => '',
    );

  }
 
  /**
   * Add form options.
   */
  public function configForm(&$form_state) {
    $form = array();
    // TODO: Specify metadata format here?
    return $form;
  }

  /**
   * Expose source form.
   */
  public function sourceForm($source_config) {
    $form = parent::sourceForm($source_config);

    // If earliest_timestamp is not set, and source is, then get info from
    // repository to populate settings.
    if ($source_config['source']) {
      require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
      $result = feeds_oai_pmh_identify($source_config['source']);
      #dpm($result);
      if ($result['status'] == 0) {
        $source_config = array_merge($source_config, $result['repository']);
      }
      else {
        drupal_set_message(t('There was a problem fetching repository information: !list', array('!list' => $result['output'])));
        return;
      }
    }

    // Build options array for sets available in repository.
    $sets_options = feeds_oai_pmh_sets_options($result['repository']['sets']);

    // Override the default "source" element provided by Feeds.
    // Clearer label and description.
    $form['source']['#title'] = t('URL of OAI-PMH endpoint');
    $form['source']['#description'] = t('You can use services like http://www.opendoar.org/ to get a list of repository OAI-PMH endpoints.');
    // Add AHAH event handler.
    $form['source']['#ahah'] = array(
      'path' => 'feeds_oai_pmh/set_ahah',
      'wrapper' => 'set-choices',  // ID of div element to update.
      'method' => 'replace',
      'effect' => 'fade',
      'event' => 'change',
    );

    if ($form['source']['#default_value']) {
      require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
      $result = feeds_oai_pmh_identify($form['source']['#default_value']);
      if ($result['status'] == 0) {
        $source_config = array_merge($source_config, $result['repository']);
      }
      else {
        $sets_options = feeds_oai_pmh_sets_options($result['repository']['sets']);
      }
    }

    $form['set'] = array(
      '#type' => 'select',
      '#title' => t('Set to fetch'),
      '#default_value' => $source_config['set'],
      '#options' => $sets_options,
      // A set wrapper to handle replacement by AHAH callback
      '#prefix' => '<div class="clear-block" id="set-choices">',
      '#suffix' => '</div>',
    );
    /*$form['last_fetched_timestamp'] = array(
      '#type' => 'textfield',
      '#title' => t('Timestamp of last fetched record'),
      '#default_value' => $source_config['last_fetched_timestamp'],
    );
     * */
    // TODO: The next information should go into a table, as it's just informative and can't be changed.
    $form['earliest_timestamp'] = array(
      '#type' => 'textfield',
      '#title' => t('Timestamp of earliest record, as reported by the repository'),
      '#default_value' => $source_config['earliest_timestamp'],
      '#disabled' => TRUE,
    );
    $form['granularity'] = array(
      '#type' => 'radios',
      '#title' => t('Granularity ("days" or "seconds") reported by repository'),
      '#default_value' => $source_config['granularity'],
      '#options' => drupal_map_assoc(array('days', 'seconds')),
      '#disabled' => TRUE,
    );
    $form['deleted_record'] = array(
      '#type' => 'textfield',
      '#title' => t('Repository support for deletedRecord'),
      '#default_value' => $source_config['deleted_record'],
      '#disabled' => TRUE,
    );
    $form['compression_gzip'] = array(
      '#type' => 'checkbox',
      '#title' => t('Repository supports gzip compression'),
      '#default_value' => $source_config['compression_gzip'],
      '#disabled' => TRUE,
    );
    $form['compression_deflate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Repository supports deflate compression'),
      '#default_value' => $source_config['compression_deflate'],
      '#disabled' => TRUE,
    );

    return $form;
  }
}
