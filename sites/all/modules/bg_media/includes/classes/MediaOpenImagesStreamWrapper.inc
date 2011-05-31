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
  protected $type;
  protected $identifier;
  protected $qualified_media = array(
    'image' => array('image/jpeg', 'image/png'),
    'audio' => array('audio/mp3'),
    'video' => array('video/mp4', 'video/ogg'),
  );

  /**
   * Override parent::setUri().
   */
  function setUri($uri) {
    parent::setUri($uri);

    // Set file type - file_uri_to_object not used because of infinite loop during node delete
    $query = db_select('file_managed', 'f')
      ->condition('f.uri', $this->uri)
      ->fields('f', array('type'));

    $type = $query->execute()->fetchField();

    // Set type
    if($type) {
      $this->type = $type;
    }

    // Set identifier
    $this->identifier = str_replace('openimages://', '', $this->uri);
  }

  /**
   * Override parent::realpath()
   * 
   * Return NULL because the file does not really exist
   */
  public function realpath() {
    return NULL;
  }
  
  static public function getMimeType($uri, $mappers = NULL) {
    return 'video/ogg';
  }

  public function getExternalUrl() {
    $media  = NULL;
    $cid    = $this->uri;
    
    if($cache = cache_get($cid, 'cache_media')) {
      $media = $cache->data;
    }
    else if(!is_null($this->type)) {
      $oai_oi_record = $this->getOriginalRecord($this->identifier);

      if($oai_oi_record) {
        $media = $this->getMedia($oai_oi_record, $this->type);
        
        if($media) {
          cache_set($cid, $media, 'cache_media', strtotime('+1 day'));
        }
      }
    }

    return $media;
  }

  public function getLocalThumbnailPath() {
    $local_path = 'public://media-openimages/' . md5($this->identifier) . '.jpg';

    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $oai_oi_record = $this->getOriginalRecord($this->identifier);

      if($oai_oi_record) {
        $media = $this->getMedia($oai_oi_record, 'image');
        
        if($media) {
          @copy($media, $local_path);
        }
      }
    }
    
    return $local_path;
  }
  
  public function getOriginalRecord($identifier) {
    $url = $this->base_url . 'feeds/oai/?' .
      http_build_query(array(
        'verb' => 'GetRecord',
        'identifier' => check_plain($identifier),
        'metadataPrefix' => 'oai_oi',
      ));

    // Fetch the feed's contents
    $result = drupal_http_request($url);

    if ($result->code == 200) {
      require_once(drupal_get_path('module', 'feeds_oai_pmh') . '/feeds_oai_pmh.inc');
      require_once(drupal_get_path('module', 'bg_feeds') . '/bg_feeds.inc');

      $xml = simplexml_load_string($result->data);
      
      return $xml->GetRecord->record;
    }
    
    return FALSE;
  }
  
  /**
   * Select file depending on requested type
   * 
   * @param unknown_type $oai_oi_record
   * @param unknown_type $type
   */
  public function getMedia($oai_oi_record, $type) {
    $media = $oai_oi_record->metadata->children('oai_oi', TRUE)->children('oi', TRUE)->medium;
    $media_amount = $media->count();
    
    // Start at back for image
    if($type == 'image') {
      foreach($this->qualified_media[$type] as $qualified_mime_type) {
        for($i = $media_amount; $i >= 0; $i--) {
          $mime_type = file_get_mimetype( (string)$media[$i] );
          
          if($mime_type == $qualified_mime_type) {
            return (string)$media[$i];
          }
        }
      }
    }
    // Otherwise start at front
    else {
      foreach($this->qualified_media[$type] as $qualified_mime_type) {
        for($i = 0; $i < $media_amount; $i++) {
          $mime_type = file_get_mimetype( (string)$media[$i] );
          
          if($mime_type == $qualified_mime_type) {
            return (string)$media[$i];
          }
        }
      }
    }
    
    return FALSE;
  }
}