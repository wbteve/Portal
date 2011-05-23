<?php

/**
 *  @file
 *  Create a Open Images Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $open_images = new ResourceOpenImagesStreamWrapper('openimages://identifier');
 */
class MediaOpenImagesStreamWrapper extends MediaReadOnlyStreamWrapper {
  protected $base_url = 'http://www.openbeelden.nl/';

  public function getTarget($f) {
    return FALSE;
  }

  static public function getMimeType($uri, $mappers = NULL) {
    return 'video/ogg';
  }

  public function getExternalUrl() {
    return str_replace('openimages://', $this->base_url, $this->uri);
  }

  public function getOriginalThumbnailPath($file_name) {
    return $this->getOriginalRecord($file_name);
  }

  public function getLocalThumbnailPath($file_name) {
    $local_path = 'public://media-openimages/' . check_plain(urlencode($file_name)) . '.jpg';
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      copy($this->getOriginalThumbnailPath($file_name), $local_path);
    }
    return $local_path;
  }
  
  public function getOriginalRecord($file_name) {
    $url = $this->base_url . 'feeds/oai/?' .
      http_build_query(array(
        'verb' => 'GetRecord',
        'identifier' => check_plain($file_name),
        'metadataPrefix' => 'oai_oi',
      ));

    // Fetch the feed's contents
    $result = drupal_http_request($url);

    if ($result->code == 200) {
      require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
      require_once(drupal_get_path('module', 'bg_feeds') . '/bg_feeds.inc');

      $xml = simplexml_load_string($result->data);
      
      foreach($xml->GetRecord->record->metadata->children('oai_oi', TRUE)->children('oi', TRUE)->medium as $media) {
        $extension = pathinfo($media, PATHINFO_EXTENSION);

        if(in_array($extension, array('jpg', 'png'))) {
          return (string)$media;
        }
      }
    }
    else {
      return FALSE;
    }
  }
}
