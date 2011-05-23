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
  protected static $extensions = array(
    'video' => array('mp4', 'ogv'),
    'audio' => array('mp3'),
    'image' => array('jpg', 'png'),
  );
  
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
          
          // Set (MIME) type
          switch($item['type']) {
            case 'Moving Image':
              $item['computedSourceMime'] = 'video/openbeelden';
              $item['computedSourceType'] = 'video';
              break;
            
            case 'Still Image':
            case 'Image':
              $item['computedSourceMime'] = 'image/openbeelden';
              $item['computedSourceType'] = 'image';
              break;
            
            case 'Sound Recording':
            case 'Sound':
              $item['computedSourceMime'] = 'audio/openbeelden';
              $item['computedSourceType'] = 'audio';
              break;
          }
          
          // Source type is unknown move to next record
          if(!isset($item['computedSourceType'])) {
            watchdog('Beeld en Geluid - Feeds', t('Unknown item of type %type in item %guid', array(
              '%type' => $item['type'], 
              '%guid' => $item['guid']
            )));
            continue;
          }
          
          $file_media = $this->getMedia($item['medium'], $item['computedSourceType']);
          
          // If medium could not be found
          if(is_null($file_media)) {
            watchdog('Beeld en Geluid - Feeds', t('No media found of type %type in item %guid', array(
              '%type' => $item['type'], 
              '%guid' => $item['guid']
            )));
            continue;
          }
          
          // Set identifier to GUID because of missing identifiers in some records
          $item['identifier'] = $item['guid'];
          
          $file_url = parse_url($file_media);
          $item['computedSource'] = 'openimages:/' . $file_url['path'];
          
          // Return the items found in feed.
          $result->items[] = $item;
        }
      }
    }

    return $result;
  }
  
  public function getMedia($medium, $type) {
    foreach($medium as $media) {
      $extension = pathinfo($media, PATHINFO_EXTENSION);

      if(in_array($extension, self::$extensions[$type])) {
        return $media;
      }
    }

    return null;
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
      'computedSource', 'computedSourceMime', 'computedSourceType'
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

function oi_feeds_set_preview(FeedsSource $source, FeedsParserResult $result, $key) {
  
}
