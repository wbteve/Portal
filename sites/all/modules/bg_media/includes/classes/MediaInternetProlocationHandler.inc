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

  public function validate() {
    // @todo Media module currently fails when two files try to have the same
    //   URI, so catch that in the validation step. Some day, it would be nice
    //   to allow it, however. See http://drupal.org/node/952422.
    $existing_files = file_load_multiple(array(), array('uri' => $this->parse($this->embedCode)));
    if (count($existing_files)) {
      throw new MediaInternetValidationException(t('You have entered a URL for media that is already in your library.'));
    }
  }
}
