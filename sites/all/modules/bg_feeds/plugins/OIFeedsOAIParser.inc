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
class OIFeedsOAIParser extends FeedsOAIParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
    $feed = feeds_oai_pmh_parse($fetcher_result->getRaw());
    // TODO: are these needed?
    #$batch->setTitle($result['title']);
    #$batch->setDescription($result['description']);
    #$batch->setLink($result['link']);
    if (!$feed) {
      // No items, return
      return;
    }

    $result = new FeedsParserResult();

    // Check for items.
    if (is_array($feed['items'])) {
      // Add set name element to each of the items.
      $oai_endpoint_url = $source->config['FeedsOAIHTTPFetcher']['source'];
      $identify_response = feeds_oai_pmh_identify($oai_endpoint_url);

      if ($identify_response['repository']['sets']) {
        foreach ($feed['items'] as $index => $item) {
          foreach ($item['setspec_raw'] as $setspec) {
            $set_name = $identify_response['repository']['sets'][$setspec]['name'];
            $feed['items'][$index]['setspec_name'][] = $set_name;
          }
          
          // Set media file preview
          $item['oi_preview'] = $item['format'][5];
          
          // Set media file source
          $file_media = parse_url($item['format'][0]);
          $item['oi_source'] = 'openbeelden:/' . $file_media['path'];
          
          // Set (MIME) type
          switch($item['type'][0]) {
            case 'Moving Image':
              $item['oi_source_mime'] = 'video/openbeelden';
              $item['oi_source_type'] = 'video';
              break;
          }

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
    $sources = parent::getMappingSources() + array(
      'oi_preview' => array(
        'name' => t('Open Images: preview'),
        'callback' => 'oi_feeds_set_preview',
        'description' => t('Media preview file URI.'),
      ),
      'oi_source' => array(
        'name' => t('Open Images: source file URI'),
        'description' => t('Media source file URI.'),
      ),
      'oi_source_mime' => array(
        'name' => t('Open Images: source MIME type'),
        'description' => t('Media source MIME type.'),
      ),
      'oi_source_type' => array(
        'name' => t('Open Images: source type'),
        'description' => t('Media source type.'),
      )
    );
    return $sources;
  }
}

function oi_feeds_set_preview(FeedsSource $source, FeedsParserResult $result, $key) {
  
}
