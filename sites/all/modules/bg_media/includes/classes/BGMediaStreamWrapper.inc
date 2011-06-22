<?php
/**
 *  @file
 *
 */

/**
 * abstract class defines standard methods for B & G Media
 */
abstract class BGMediaStreamWrapper extends MediaReadOnlyStreamWrapper {
  public function getLocalThumbnailPath() {
    return NULL;
  }

  public function getMultiExternalUrl() {
    $media = array(get_object_vars(file_uri_to_object($this->getExternalUrl())));
    return $media;
  }
}
