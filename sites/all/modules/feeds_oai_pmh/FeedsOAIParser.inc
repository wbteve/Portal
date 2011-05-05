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
class FeedsOAIParser extends FeedsParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
    $result = feeds_oai_pmh_parse($batch->getRaw());
    // TODO: are these needed?
    #$batch->setTitle($result['title']);
    #$batch->setDescription($result['description']);
    #$batch->setLink($result['link']);
    if (!$result) {
      // No items, return
      return;
    }

    // Check for items.
    if (is_array($result['items'])) {
      
      // Add set name element to each of the items.
      $oai_endpoint_url = $source->config['FeedsOAIHTTPFetcher']['source'];
      $identify_response = feeds_oai_pmh_identify($oai_endpoint_url);
      if ($identify_response['repository']['sets']) {
        foreach ($result['items'] as $index => $item) {
          foreach ($item['setspec_raw'] as $setspec) {
            $set_name = $identify_response['repository']['sets'][$setspec]['name'];
            $result['items'][$index]['setspec_name'][] = $set_name;
          }
        }
      }
      
      // Return the items found in feed.
      $batch->setItems($result['items']);
    }
  }

  /**
   * Implementation of FeedsParser::clear().
   *
   * Delete variables containing resumptionToken and from used in last fetch.
   */
  public function clear(FeedsSource $source) {
    // Only if FeedsOAIHTTPFetcher was used as the fetcher.
    if ($source->config['FeedsOAIHTTPFetcher']) {
      $oai_endpoint_url = $source->config['FeedsOAIHTTPFetcher']['source'];
      $set = $source->config['FeedsOAIHTTPFetcher']['set'];
      variable_del('feeds_oai:resumptionToken:' . $set . ':' . $oai_endpoint_url);
      variable_del('feeds_oai:from:' . $set . ':' . $oai_endpoint_url);
    }
    parent::clear($source);
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

    // Add dublin core field to mapping sources
    $elements = array(
      'title', 'type', 'subject', 'contributor', 'creator', 'description', 'publisher',
      'date',  'format', 'identifier', 'source', 'language', 'relation',
      'coverage', 'rights');
    foreach ($elements as $element) {
      $sources[$element] = array(
        'name' => t('Metadata: dc:@element', array('@element' => $element)),
        'description' => t('From the metadata record.'),
      );
    }
    
    return $sources;
  }
}
