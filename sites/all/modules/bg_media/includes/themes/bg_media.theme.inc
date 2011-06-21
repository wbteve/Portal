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
    // Add the videojs library to the page.
    videojs_add();
    drupal_add_library('bg_media', 'swfobject');

    $vars['videos']           = array();
    $vars['thumbnail']        = get_object_vars(file_uri_to_object($wrapper->getLocalThumbnailPath()));
    $vars['video_candidates'] = $wrapper->getMediaForDisplay();

    $codecs = array(
      'video/mp4'                 => "video/mp4; codecs='avc1.42E01E, mp4a.40.2'",
      'video/webm'                => 'video/webm; codec="vp8, vorbis"',
      'application/octet-stream'  => 'video/webm; codec="vp8, vorbis"',
      'video/ogg'                 => 'video/ogg; codec="theora, vorbis"',
      'application/ogg'           => 'video/ogg; codec="theora, vorbis"',
      'video/quicktime'           => 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"'
    );

    foreach($vars['video_candidates'] as &$video_candidate) {
      // Skip unplayable items.
      if (!isset($codecs[$video_candidate['filemime']])) {
        continue;
      }

      $video_candidate['video_url']   = file_create_url($video_candidate['uri']);
      $video_candidate['video_type']  = "video/mp4; codecs='avc1.42E01E, mp4a.40.2'";

      $vars['flash_video'] = <<<HTML
<object id="flowplayer" class="vjs-flash-fallback" width="480" height="360" data="http://releases.flowplayer.org/swf/flowplayer-3.2.7.swf" type="application/x-shockwave-flash">
  <param name="movie" value="http://releases.flowplayer.org/swf/flowplayer-3.2.7.swf" />
  <param name="allowfullscreen" value="true" />
  <param name="flashvars" value='config={"clip":"{$clip}"}' />
</object>
HTML;

      $vars['videos'][] = $video_candidate;
    }

    // set the width and height
    $vars['width']  = 480;
    $vars['height'] = 360;
  }
}