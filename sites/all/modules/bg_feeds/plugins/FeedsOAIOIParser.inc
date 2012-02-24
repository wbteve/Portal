<?php

/**
 * @file
 * Implementation of FeedsParser::parse().
 *
 */

/**
 * Class definition for Common Syndication Parser.
 *
 * Parses RSS and Atom feeds.
 */
class FeedsOAIOIParser extends FeedsOAIParser {
  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
    require_once(drupal_get_path('module', 'bg_feeds') . '/bg_feeds.inc');

    $feed = bg_feeds_oai_oi_parse($fetcher_result->getRaw());

    // TODO: are these needed?
    #$batch->setTitle($result['title']);
    #$batch->setDescription($result['description']);
    #$batch->setLink($result['link']);
    if (!$feed) {
      // No items, return empty result
      return new FeedsParserResult();
    }

    $result = new FeedsParserResult();

    // Check for items.
    if (is_array($feed['items'])) {
      // Add set name element to each of the items.
      $oai_endpoint_url = $source->config['FeedsOAIOIHTTPFetcher']['source'];
      $identify_response = feeds_oai_pmh_identify($oai_endpoint_url);

      if ($identify_response['repository']['sets']) {
        foreach ($feed['items'] as $index => $item) {
          foreach ($item['setspec_raw'] as $setspec) {
            $set_name = $identify_response['repository']['sets'][$setspec]['name'];
            $feed['items'][$index]['setspec_name'][] = $set_name;
          }

          foreach ($item as $key => &$data) {
            if(is_array($data) && !in_array($key, array('subject', 'publisher', 'medium', 'spatial'))) {
              $data = array_shift($data);
            }
          }

          $item['importSource'] = 'Open Images';

          $interval = new DateInterval($item['extent']);
          $item['extent'] = bg_feeds_hms_to_seconds($interval->format('%H:%I:%S'));

          $guid = str_replace(':', '.', $item['guid']);

          $item['computedSource'] = "openimages://{$guid}/{$item['type']}";

          // Set identifier to GUID because of missing identifiers in some records
          $item['identifier'] = $item['guid'];

          // Return the items found in feed.
          $result->items[] = $item;
        }
      }
    }

    return $result;
  }

  /**
   * Return mapping sources.
   */
  public function getMappingSources() {
    self::loadMappers();
    $sources = array();
    drupal_alter('feeds_parser_sources', $sources, feeds_importer($this->id)->config['content_type']);
    $sources += array(
      'guid' => array(
        'name' => t('Repository Record identifier'),
        'description' => t('A unique string per each metadata record, defined by the repository.'),
      ),
      'timestamp' => array(
        'name' => t('Record publication date'),
        'description' => t("Date this metadata record was published on the repository. Different from the described item's publication date."),
      ),
      'url' => array(
        'name' => t('URL to resource'),
        'description' => t('All valid URLs detected in dc:identifier elements.'),
      ),
      'setspec_raw' => array(
        'name' => t('Set: setSpec (raw value)'),
        'description' => t("The set/setSpec from the record's header."),
      ),
      'setspec_name' => array(
        'name' => t('Set: name'),
        'description' => t("The set name for this record, taken from the repository's identify response."),
      ),
    );

    // Add Open Images field to mapping sources
    $elements = array(
      // Default set elements (see: http://www.openbeelden.nl/api)
      'title', 'alternative', 'creator', 'subject',  'description', 'abstract',
      'publisher', 'contributor', 'date', 'type', 'extent', 'medium',
      'identifier', 'source', 'language', 'references', 'spatial', 'attributionName',
      'attributionURL', 'license',
      // Computed fields
      'computedSource', 'importSource',
      );
    foreach ($elements as $element) {
      $sources[$element] = array(
        'name' => t('Metadata: oi:@element', array('@element' => $element)),
        'description' => t('From the metadata record.'),
      );
    }

    return $sources;
  }
}

/**
 * Enclosure element, can be part of the result array.
 */
class FeedsOAIOIEnclosure extends FeedsEnclosure {
  protected $type;

  /**
   * Constructor, requires MIME type.
   *
   * @param $value
   *   A path to a local file or a URL to a remote document.
   * @param $mimetype
   *   The mime type of the resource.
   */
  public function __construct($value, $type) {
    parent::__construct($value, $type);

    // Set (MIME) type
    switch($type) {
      case 'Moving Image':
        $this->mime_type = 'video/openbeelden';
        $this->type = 'video';
        break;

      case 'Still Image':
      case 'Image':
        $this->mime_type = 'image/openbeelden';
        $this->type = 'image';
        break;

      case 'Sound Recording':
      case 'Sound':
        $this->mime_type = 'audio/openbeelden';
        $this->type = 'audio';
        break;
    }
  }

  public function getType() {
    return $this->type;
  }

  /**
   * Get a Drupal file object of the enclosed resource, download if necessary.
   *
   * @param $destination
   *   The path or uri specifying the target directory in which the file is
   *   expected. Don't use trailing slashes unless it's a streamwrapper scheme.
   *
   * @return
   *   A Drupal temporary file object of the enclosed resource.
   *
   * @throws Exception
   *   If file object could not be created.
   */
  public function getFile($destination) {
    $file = NULL;

    if ($this->getValue() && !is_null($this->type)) {
      $file = file_uri_to_object($this->getValue());
      // Override file MIME
      $file->filemime = $this->mime_type;
      // Append file entity type
      $file->type = $this->type;

      file_save($file);

      // We couldn't make sense of this enclosure, throw an exception.
      if (!$file) {
        throw new Exception(t('Invalid enclosure %enclosure', array('%enclosure' => $this->getValue())));
      }
    }

    return $file;
  }
}

function oi_feeds_set_preview(FeedsSource $source, FeedsParserResult $result, $key) {

}
