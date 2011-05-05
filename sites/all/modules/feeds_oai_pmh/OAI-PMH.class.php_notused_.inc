<?php


/**
 * Function with a sample use of the OAI class.
 */
function OAI_example_call_xyz() {
  $oai_service_url = 'http://www.dlese.org/oai/provider';
  $oai = new OAI($oai_service_url);

  // Optional: configure object to use a certain set
  if ($sets = $oai->getSets()) {
    // Display # of found sets.
    echo "Repository has " . sizeof($sets) . " sets.\n";

    // Set fetching to the first set in array.
    $set = array_shift($sets);
    echo "Selecting first set:\n";
    print_r($set);
    $oai->setCurrentSet($set['setSpec']);
  }
  
  // Fetch XML response for all records, starting from beginning.
  $raw_xml = $oai->requestRawListRecords();
  // Parse the response
  $result = OAI::parseListRecordsOAI_DC($raw_xml);
  
  // Show some data.
  echo sizeof($result['items']) . " valid records.\n";
  echo sizeof($result['deleted_items']) . " deleted records.\n";
  echo "completeListSize: " . $result['completeListSize'] . "\n";
  echo "resumptionToken: " . $result['resumptionToken'] . "\n";
}

/**
 * Class to connect to and process data from OAI-PMH repositories.
 * See http://www.openarchives.org/OAI/openarchivesprotocol.html
 */
class OAI {
  protected $oai_service_url;
  protected $granularity;
  protected $sets;
  protected $current_set = '';
  protected $repository;

  public function __construct($oai_service_url, $repository = NULL) {
    $this->oai_service_url = $oai_service_url;

    if (!$repository) {
      // If no repository specs given, request it from OAI-PMH service.
      $response = $this->requestParsedIdentify();
      if ($response['status'] != 0) {
        return FALSE;
      }
      $repository = $response['repository'];
    }
    $this->repository = $repository;
    $this->granularity = $repository['granularity'];
    $this->sets = $repository['sets'];
  }

  public function setCurrentSet($set) {
    $this->current_set = $set;
  }
  public function getSets() {
    return $this->sets;
  }
  public function getGranularity() {
    return $this->granularity;
  }
  public function getRepository() {
    return $this->repository;
  }

  /**
   * Returns an array of information returned by the OAI-PMH Identify verb.
   */
  public function requestParsedIdentify() {
    $output = array();
    $request = "verb=Identify";
    $repository = array(
      'deleted_record' => '',
      'compression' => FALSE,
      'compression_gzip' => FALSE,
      'compression_deflate' => FALSE,
      'earliest_timestamp' => '',
      'sets' => array(),
    );

    $result = $this->_httpOaiRequest($request);
    if ($result->code != 200) {
      $message = t('OAI repository %repo at url %url is not avaliable, please check URL is correct.', array('%repo' => $baseurl, '%url' => $baseurl));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }

    // Returns FALSE on error
    $xml = @simplexml_load_string($result->data);
    if (!$xml) {
      $message = t('OAI repository %repo returns invalid XML upon identify.', array('%repo' => $baseurl));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }

    $ident = $xml->Identify;
    // Things which must come back, or return error
    // Protocool Version
    if ($ident->protocolVersion != '2.0') {
      $message = t('OAI repository %repo: Incorrect Identify Response -- Unsupported Protcool Version "@version"', array('%repo' => $baseurl, '@version' => $ident->protocolVersion));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }
    else {
      $repository["protocol_version"] = (string)$ident->protocolVersion;
    }

    // DeleteRecord
    if (!isset($ident->deletedRecord)) {
      $message = t('OAI repository %repo: Incorrect Identify Response -- No deleteRecord', array('%repo' => $baseurl));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }
    else {
      $repository['deleted_record'] = (string)$ident->deletedRecord;
    }

    // earliest Datestamp
    if (!isset($ident->earliestDatestamp)) {
      $message = t('OAI repository %repo: Incorrect Identify Response -- No earliest Datestamp', array('%repo' => $baseurl));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }
    else {
      $repository['earliest_timestamp'] = strtotime((string)$ident->earliestDatestamp);
    }

    // Granularity
    if (!isset($ident->granularity)) {
      $message = t('OAI repository %repo: Incorrect Identify Response -- No Granularity', array('%repo' => $baseurl));
      $this->log($message);
      return array(
        'output' => $message,
        'status' => 1,
      );

    }
    else {
      // Granularty is only in days
      // Magic number from strlen(YYYY-MM-DD)
      if (strlen($ident->granularity) == 10) {
        $repository['granularity'] = 'days';
      }
      // Granularity is in seconds
      // Magic number from strlen(YYYY-MM-DDThh:mm:ssZ)
      elseif (strlen($ident->granularity) == 20) {
        $repository['granularity'] = 'seconds';
      }
      else {
        $message = t('OAI repository %repo: Incorrect Identify Response -- Invalid granularity', array('%repo' => $baseurl));
        $this->log($message);
        return array(
          'output' => $message,
          'status' => 1,
        );
      }
    }

    // Optional things, which are nice to have
    if (isset($ident->compression)) {
      // According to HTTP 1.1 RFC 2616 there is also the Lempel-Ziv-Welch
      // compression, which in theory could be supported. However, PHP doesn't
      // seem to play nice with it, and I haven't seen a repo with it. It is also
      // 14 years old.
      $repository['compression'] = TRUE;
      foreach ($ident->compression as $encoding) {
        if ($encoding == 'gzip') {
          $repository['compression_gzip'] = TRUE;
        }
        elseif ($encoding == 'deflate') {
          $repository['compression_deflate'] = TRUE;
        }
      }
    }

    // Get and assign sets information
    if ($sets = $this->requestParsedListSets()) {
      $repository['sets'] = $sets;
    }

    $return = array('output' => $output, 'status' => 0, 'repository' => $repository);
    return $return;
  }

  /**
   * Generate request arguments for a ListRecords operation.
   *
   * @param $from_timestamp
   *   Optional. A timestamp indicating the from parameter in a ListRecords
   *   request. Ignored if $resumption_token is set.
   * @param $until_timestamp
   *   Optional. A timestamp indicating the until parameter in a ListRecords
   *   request. Ignored if $resumption_token is set.
   * @param $resumption_token
   *   The value of a resumptionToken element from a previous response. If
   *   given, $from_timestamp and $until_timestamp will be ignored.
   */
  public function argsListRecords($resumption_token = NULL, $metadata_prefix = 'oai_dc', $from_timestamp = '', $until_timestamp = '') {
    $args = 'verb=ListRecords';
    if ($resumption_token) {
      $args .= "&resumptionToken=" . rawurldecode($resumption_token);
    }
    else {
      // When a resumptionToken is issued, there can't be any other arguments
      // in the request.
      $args .= '&metadataPrefix=' . $metadata_prefix;
      if ($from_timestamp) {
        $args .= '&from=' . rawurlencode($this->_formatDate($from_timestamp, $granularity));
      }
      if ($until_timestamp) {
          $args .= '&until=' . rawurlencode($this->_formatDate($until_timestamp, $granularity));
        }
      if ($this->current_set) {
        $args .= '&set=' . rawurlencode($this->current_set);
      }
    }
    return $args;
  }

  /**
   * Returns a raw XML response from a listRecords operation.
   *
   * @see argsListRecords()
   */
  public function requestRawListRecords($resumption_token = NULL, $metadata_prefix = 'oai_dc', $from_timestamp = '', $until_timestamp = '') {
    $args = OAI::argsListRecords($resumption_token, $metadata_prefix, $from_timestamp, $until_timestamp);
    $result = $this->_httpOaiRequest($args);
    if ($result->code == 200) {
      // Return the feed's contents
      return $result->data;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Parse a raw ListRecords response from an OAI-PMH endpoint into an array of items.
   *
   * Contents of returned array:
   *   'items' => $items,
   *   'deleted_items' => $deleted_items,
   *   'resumptionToken' => $resumption_token,
   *   'completeListSize' => $complete_list_size,
   *   'cursor' => $cursor
   * 
   * @return
   *   An array containing parsed items.
   */
  public function parseListRecordsOAI_DC($raw_xml) {
    // Items array
    $items = array();
    $deleted_items = array();

    // Parse raw string into xml
    $xml = @simplexml_load_string($raw_xml);
    if (!$xml) {
      return FALSE;
    }

    // If error element is set, we have a problem.
    if (isset($xml->error)) {
      return FALSE;
    }

    // Process each <record> element in the response.
    foreach ($xml->ListRecords->record as $xml_item) {

      // Handle items marked deleted
      if ($xml_item->header["status"] == "deleted") {
        $deleted_items[] = array(
          'guid' => (string)$xml_item->header->identifier,
        );
        continue;
      }

      $xml_dc_metadata = $xml_item->metadata->children('http://www.openarchives.org/OAI/2.0/oai_dc/')->children('http://purl.org/dc/elements/1.1/');
      $item = array(
        'guid' => (string)$xml_item->header->identifier,
        'datestamp' => strtotime((string)$xml_item->header->datestamp),
        'title' => (string)$xml_dc_metadata->title,
      );
      $elements = array(
        'creator', 'subject', 'description', 'publisher', 'contributor',
        'date', 'type', 'format', 'identifier', 'source', 'language', 'relation',
        'coverage', 'rights');
      foreach ($elements as $element) {
        $item[$element] = array();
        if (isset($xml_dc_metadata->$element)) {
          foreach ($xml_dc_metadata->$element as $value) {
            $value = (string)$value;
            // TODO: add callback functions to further process values (like convert dates to timestamps, split subjects, etc.)
            $item[$element][$value] = $value;
          }
          $item[$element] = array_values($item[$element]);
        }
      }

      // Add "url" element from "identifier" items that are URLs.
      foreach ($item['identifier'] as $value) {
        if (OAI::validUrl($value)) {
          $item['url'][] = $value;
        }
//        else {
//          dsm("URL $value not valid!");
//        }
      }

      // Add this item to the items array.
      $items[] = $item;
    }

    // Handle resumptionToken element in response.
    //
    // Sample resumptionToken signaling end of list:
    //   <resumptionToken completeListSize="6" cursor="4"/>
    //
    // Sample resumptionToken indicating there are more items left:
    //
    //   <resumptionToken expirationDate="2002-06-01T23:20:00Z"
    //     completeListSize="6"
    //     cursor="0">xxx45abttyz</resumptionToken>
    if (isset($xml->ListRecords->resumptionToken)) {
      $resumption_token = (string)$xml->ListRecords->resumptionToken;
      $attributes = $xml->ListRecords->resumptionToken->attributes();
      $complete_list_size = $attributes['completeListSize'];
      $cursor = $attributes['cursor'];
    }
    else {
      $resumption_token = '';
      $complete_list_size = '';
      $cursor = '';
    }

    return array(
      'items' => $items,
      'deleted_items' => $deleted_items,
      'resumptionToken' => $resumption_token,
      'completeListSize' => $complete_list_size,
      'cursor' => $cursor,
    );
  }
  
  /**
   * Returns an array populated with the avaiable sets reported by an OAI-PMH endpoint.
   */
  public function requestParsedListSets() {
    $sets = array();
    $args = "verb=ListSets";
    $result = $this->_httpOaiRequest($args);

    // Return false on error
    if ($result->code != 200) {
      return FALSE;
    }
    $xml = simplexml_load_string($result->data);
    if (!$xml) {
      return FALSE;
    }
    if (isset($xml->error)) {
      return FALSE;
    }

    // Put set names into $sets array
    foreach ($xml->ListSets->set as $set) {
      $sets[(string)$set->setSpec]['setSpec'] = (string)$set->setSpec;
      $sets[(string)$set->setSpec]['name'] = (string)$set->setName;
      if ($set->setDescription) {
        // TODO: Use SimpleXML instead of regexp
        $description = $set->setDescription->asXML();
        $description = preg_replace('/.*?<dc:description>([^<]+)<.dc:description.*/s', '\1', $description);
        $sets[(string)$set->setSpec]['description'] = $description;
      }
    }
    return $sets;
  } 
  
  /**
   * Helper function that converts a timestamp into the repository's date format.
   */
  protected function _formatDate($timestamp) {
    $granularity = $this->granularity;
    if ('seconds' == $granularity) {
      $date_format = 'Y-m-d\TH:m:s\Z';
    }
    elseif ('days' == $granularity) {
      $date_format = 'Y-m-d';
    }
    return date($date_format, $timestamp);
  }

  /**
   * Issues a request.
   */
  protected function _httpOaiRequest($args) {
    $url = $this->oai_service_url;
    $url .= '?' . $args;
    $data = @file_get_contents($url);
    #echo $url . "\n";
    if ($data) {
      return (object)array('code' => 200, 'data' => $data);
    } else {
      return (object)array('code' => 500);
    }
  }

  protected function log($msg) {
    echo $msg . "\n";
  }

  protected function validUrl($url) {
    return (bool)preg_match("
      /^                                                      # Start at the beginning of the text
      (?:ftp|https?):\/\/                                     # Look for ftp, http, or https schemes
      (?:                                                     # Userinfo (optional) which is typically
        (?:(?:[\w\.\-\+!$&'\(\)*\+,;=]|%[0-9a-f]{2})+:)*      # a username or a username and password
        (?:[\w\.\-\+%!$&'\(\)*\+,;=]|%[0-9a-f]{2})+@          # combination
      )?
      (?:
        (?:[a-z0-9\-\.]|%[0-9a-f]{2})+                        # A domain name or a IPv4 address
        |(?:\[(?:[0-9a-f]{0,4}:)*(?:[0-9a-f]{0,4})\])         # or a well formed IPv6 address
      )
      (?::[0-9]+)?                                            # Server port number (optional)
      (?:[\/|\?]
        (?:[\w#!:\.\?\+=&@$'~*,;\/\(\)\[\]\-]|%[0-9a-f]{2})   # The path and query (optional)
      *)?
    $/xi", $url);
  }

}