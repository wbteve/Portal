<?php
/**
 *  @file
 *  Create a Barend en Van Dorp Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $bvd = new ResourceOpenImagesStreamWrapper('barendenvandorp://identifier');
 */
class MediaBVDStreamWrapper extends MediaReadOnlyStreamWrapper {
  protected $base_url = 'rtmpe://stream10.bevd.prolocation.net/simplevideostreaming/';
  
  public function getExternalUrl() {
    return str_replace('barendenvandorp://', $this->base_url, $this->uri);
  }
  
  public function getLocalThumbnailPath() {
    
  }
}
