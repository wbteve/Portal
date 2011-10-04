<?php
/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers().
 */
class MediaInternetProlocationHandler extends MediaInternetBaseHandler {
  public function parse($embedCode) {
    if(preg_match('@rtmpe://stream10.bevd.prolocation.net/simplevideostreaming/(.+)@i', $embedCode, $matches) && isset($matches[1])) {
      return file_stream_wrapper_uri_normalize('prolocation://' . $matches[1]);
    }
  }

  public function claim($embedCode) {
    if ($this->parse($embedCode)) {
      return TRUE;
    }
  }

  public function save() {
    $file = $this->getFileObject();
    file_save($file);
    return $file;
  }

  public function getFileObject() {
    $uri = $this->parse($this->embedCode);
    //@todo: this is terribly broken in some ways because the function is really
    // made for local files which are 'real'
    return file_uri_to_object($uri);
  }
}
