<?php
/**
 *  @file
 *  Create a Barend en Van Dorp Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $bvd = new ResourceOpenImagesStreamWrapper('barendenvandorp://identifier');
 */
class MediaBVDStreamWrapper extends BGMediaStreamWrapper {
  protected $base_url = 'rtmpe://stream10.bevd.prolocation.net/simplevideostreaming/';
  protected $image_base_url = 'http://www.barendenvandorp.nl/sites/default/files/stills/';

  public function getBaseUrl() {
    return substr($this->base_url, 0, -1);
  }

  public function getExternalUrl() {
    return str_replace('barendenvandorp://', $this->base_url, $this->uri);
  }

  public function getExternalPath() {
    return str_replace('barendenvandorp://', 'mp4:', $this->uri);
  }

  public function getLocalThumbnailPath() {
    $local_path = 'public://media-barendenvandorp/' . md5($this->uri) . '.jpg';

    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $media = str_replace(array('barendenvandorp://', '.mp4'), array($this->image_base_url, '.jpeg'), $this->uri);
      @copy($media, $local_path);
    }

    return $local_path;
  }
}
