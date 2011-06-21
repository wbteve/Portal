<?php

/**
 * @file bg_media/includes/themes/bg_media.theme.inc
 *
 * Theme and preprocess functions for Beeld en Geluid media.
 */

/**
 * Preprocess function for theme('bg_media_video').
 */
function bg_media_preprocess_bg_media_video(&$vars) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($vars['file']->uri);
  
  if($wrapper instanceof BGMediaStreamWrapper) {
    $vars['thumbnail'] = get_object_vars(file_uri_to_object($wrapper->getLocalThumbnailPath()));
    $vars['video'] = get_object_vars(file_uri_to_object($wrapper->getExternalUrl()));
  }
}