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
  
  $variables['file']->filemime = $wrapper->getMimeType($variables['file']->uri);
  
  // Create thumbnail
  $variables['thumbnail'] = new stdClass();
  $variables['thumbnail']->uri = $wrapper->getLocalThumbnailPath($variables['file']->filename);
}

function theme_bg_media_video(&$variables) {
  $video = $variables['file'];
  $thumbnail = $variables['thumbnail'];
  
  return theme('videojs', array(
    'items' => array(
      get_object_vars($thumbnail),
      get_object_vars($video) 
    )
  ));
}