<?php

/**
 * @file bg_media/includes/themes/bg_media.theme.inc
 *
 * Theme and preprocess functions for Beeld en Geluid media.
 */

/**
 * Preprocess function for theme('bg_media_video').
 */
function bg_media_preprocess_bg_media_video(&$variables) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($variables['file']->uri);
  
  if($wrapper instanceof MediaReadOnlyStreamWrapper) {
    $variables['items'] = array(
      get_object_vars(file_uri_to_object($wrapper->getLocalThumbnailPath())),
      get_object_vars(file_uri_to_object($wrapper->getExternalUrl())),
    );
  }
}

function theme_bg_media_video(&$variables) {
  return theme('videojs', $variables);
}