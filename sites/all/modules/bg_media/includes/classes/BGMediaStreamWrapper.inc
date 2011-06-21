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
}
