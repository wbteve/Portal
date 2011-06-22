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
    $js_path = base_path() . drupal_get_path('module', 'bg_media') . '/includes/js';
    $fallback_video_url = null;

    // Add the videojs library to the page.
    drupal_add_library('bg_media', 'videojs');
    drupal_add_library('bg_media', 'swfobject');
    drupal_add_js($js_path . '/flowplayer/example/flowplayer-3.2.6.min.js');
    drupal_add_js($js_path . '/bg_media.js');
    drupal_add_js(array(
      'bgMedia' => array(
        'playerId'      => BG_MEDIA_FALLBACK_PLAYER_ID,
        'playerSwf'     => $js_path . '/flowplayer/flowplayer-3.2.7.swf',
        'playerWidth'   => 480,
        'playerHeight'  => 360,
        'playerAttr'    => array(
          'class'         => 'vjs-flash-fallback'
        ),
      ),
    ), 'setting');

    $vars['videos']           = array();
    $vars['thumbnail']        = file_create_url($wrapper->getLocalThumbnailPath());
    $vars['video_candidates'] = $wrapper->getMultiExternalUrl();

    $codecs = array(
      'video/mp4'                 => "video/mp4; codecs='avc1.42E01E, mp4a.40.2'",
      'video/webm'                => 'video/webm; codec="vp8, vorbis"',
      'application/octet-stream'  => 'video/webm; codec="vp8, vorbis"',
      'video/ogg'                 => 'video/ogg; codec="theora, vorbis"',
      'application/ogg'           => 'video/ogg; codec="theora, vorbis"',
      'video/quicktime'           => 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"'
    );

    foreach($vars['video_candidates'] as &$video_candidate) {
      $video_candidate = get_object_vars(file_uri_to_object($video_candidate));

      // Skip unplayable items.
      if (!isset($codecs[$video_candidate['filemime']])) {
        continue;
      }

      $video_candidate['video_url']   = file_create_url($video_candidate['uri']);
      $video_candidate['video_type']  = $codecs[$video_candidate['filemime']];

      $vars['flash_video'] = '<div id="' . BG_MEDIA_FALLBACK_PLAYER_ID . '"></div>';

      // Set fallback video
      if($video_candidate['filemime'] == 'video/mp4' && empty($fallback_video_url)) {
        $fallback_video_url = $video_candidate['video_url'];
      }

      $vars['videos'][] = $video_candidate;
    }

    $config = drupal_json_encode(
      array(
        'playlist'  => array(
          array(
            'url'     => $vars['thumbnail'],
          ),
          array(
            'url'       => $fallback_video_url,
            'autoPlay'  => false,
          )
        )
      )
    );
    $config = str_replace("\"", "'", $config);

    drupal_add_js(array(
      'bgMedia' => array(
        'playerFlVars'  => array(
          'config' => $config
        ),
      ),
    ), 'setting');

    // set the width and height
    $vars['width']  = 480;
    $vars['height'] = 360;
  }
}