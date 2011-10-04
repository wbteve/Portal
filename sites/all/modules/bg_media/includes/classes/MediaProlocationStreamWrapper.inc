<?php
/**
 *  @file
 *  Create a Barend en Van Dorp Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $prolocation = new ResourceStreamWrapper('prolocation://identifier');
 */
class MediaProlocationStreamWrapper extends BGMediaStreamWrapper {
  protected $base_url       = 'rtmpe://stream10.bevd.prolocation.net/simplevideostreaming/';
  protected $image_base_url = 'http://files.beeldengeluid.nl/video/keyframes/prolocation/';

  static function getMimeType($uri, $mapping = NULL) {
    return 'video/prolocation';
  }

  public function getBaseUrl() {
    return substr($this->base_url, 0, -1);
  }

  public function getExternalUrl() {
    return str_replace('prolocation://', $this->base_url, $this->uri);
  }

  public function getExternalPath() {
    return str_replace('prolocation://', 'mp4:', $this->uri);
  }

  public function getLocalThumbnailPath() {
    $local_path = 'public://media-prolocation/' . md5($this->uri) . '.jpg';

    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $media = str_replace(array('prolocation://', '.mp4'), array($this->image_base_url, '.jpeg'), $this->uri);
      @copy($media, $local_path);
    }

    return $local_path;
  }
}
