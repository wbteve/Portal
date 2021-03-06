<?php

/**
 * @file bg_media/includes/themes/bg_media.theme.inc
 *
 * Theme and preprocess functions for Beeld en Geluid media.
 *
 * @TODO: Refactor and split up copy/pasted code
 */

/**
 * Preprocess function for theme('bg_media_video').
 */
function bg_media_preprocess_bg_media_video(&$vars) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($vars['file']->uri);
  $js_path = drupal_get_path('module', 'bg_media') . '/includes/js';
  drupal_add_js($js_path . '/bg_media.js');

  $player_id = drupal_html_id(BG_MEDIA_FLASH_PLAYER_ID);

  // Add the videojs library to the page.
  bg_media_load_videojs();

  if($wrapper instanceof MediaYouTubeStreamWrapper) {
    $vars['video'] = theme('media_youtube_video', array('uri' => $vars['file']->uri, 'width' => $vars['width'], 'height' => $vars['height']));
  }
  // Theming of Barend en Van Dorp items
  if($wrapper instanceof MediaBVDStreamWrapper || $wrapper instanceof MediaProlocationStreamWrapper) {
    $vars['video'] = '<div id="' . $player_id . '" class="flowplayer ' . BG_MEDIA_FLASH_PLAYER_ID . '"></div>';
    flowplayer_add('#' . $player_id, array(
      'playlist'  => array(
        array(
          'url'     => file_create_url($wrapper->getLocalThumbnailPath()),
        ),
        array(
          // Replace single quote in URL for IE compatibility
          'url'       => str_replace("'", '%2527', $wrapper->getExternalPath()),
          'provider'  => 'rtmp',
          'autoPlay'  => false,
          'onStart'	  => 'Drupal.BGMediaPlayerPlayHandler',
          'onResume'  => 'Drupal.BGMediaPlayerPlayHandler',
          'onStop'	  => 'Drupal.BGMediaPlayerStopHandler',
          'onPause'   => 'Drupal.BGMediaPlayerStopHandler',
        ),
      ),
      'plugins'   => array(
        'rtmp'  => array(
          'url'               => 'flowplayer.rtmp.swf',
          'netConnectionUrl'  => $wrapper->getBaseUrl(),
        )
      ),
    ));
  }
  // Theming of Open Images items
  else if($wrapper instanceof BGMediaStreamWrapper) {
    $fallback_video_url = null;

    $vars['videos']           = array();
    $vars['thumbnail']        = file_create_url($wrapper->getLocalThumbnailPath());
    $vars['video_candidates'] = $wrapper->getMultiExternalUrl();

    // Extract fallback video and HTML5 playable video's
    bg_media_get_videos($vars['video_candidates'], $vars['videos'], $fallback_video_url);

    $vars['flash_video'] = '<div id="' . $player_id . '" class="flowplayer vjs-flash-fallback"></div>';
    flowplayer_add('#' . $player_id, array(
      'playlist'  => array(
        array(
          'url'       => $vars['thumbnail'],
        ),
        array(
          // Replace single quote in URL for IE compatibility
          'url'       => str_replace("'", '%2527', $fallback_video_url),
          'autoPlay'  => false,
          'onStart'	  => 'Drupal.BGMediaPlayerPlayHandler',
          'onResume'  => 'Drupal.BGMediaPlayerPlayHandler',
          'onStop'	  => 'Drupal.BGMediaPlayerStopHandler',
          'onPause'   => 'Drupal.BGMediaPlayerStopHandler',
        ),
      ),
      'onBeforeLoad'  => 'Drupal.BGMediaPlayerLoadHandler',
    ));
  }
  else if($wrapper instanceof DrupalPublicStreamWrapper) {
    $icon_path    = media_variable_get('icon_base_directory') . '/' . media_variable_get('icon_set');
    $poster_path  = base_path() . file_icon_path((object)$vars['file'], $icon_path);
    $fallback_video_url = null;

    $vars['videos']           = array();
    $vars['thumbnail']        = file_create_url($poster_path);
    $vars['video_candidates'] = array($vars['file']->uri);

    // Extract fallback video and HTML5 playable video's
    bg_media_get_videos($vars['video_candidates'], $vars['videos'], $fallback_video_url);

    $vars['flash_video'] = '<div id="' . $player_id . '" class="flowplayer vjs-flash-fallback"></div>';
    flowplayer_add('#' . $player_id, array(
      'playlist'  => array(
        array(
          'url'       => $vars['thumbnail'],
        ),
        array(
          // Replace single quote in URL for IE compatibility
          'url'       => str_replace("'", '%2527', $fallback_video_url),
          'autoPlay'  => false,
          'onStart'	  => 'Drupal.BGMediaPlayerPlayHandler',
          'onResume'  => 'Drupal.BGMediaPlayerPlayHandler',
          'onStop'	  => 'Drupal.BGMediaPlayerStopHandler',
          'onPause'   => 'Drupal.BGMediaPlayerStopHandler',
        ),
      ),
      'onBeforeLoad'  => 'Drupal.BGMediaPlayerLoadHandler',
    ));
  }
}

/**
 * Preprocess function for theme('bg_media_audio').
 */
function bg_media_preprocess_bg_media_audio(&$vars) {
  $js_path      = drupal_get_path('module', 'bg_media') . '/includes/js';
  drupal_add_js($js_path . '/bg_media.js');

  $player_id = drupal_html_id(BG_MEDIA_FLASH_PLAYER_ID);

  $wrapper      = file_stream_wrapper_get_instance_by_uri($vars['file']->uri);

  $vars['flash_audio'] = '<div id="' . $player_id . '" class="flowplayer ' . BG_MEDIA_FLASH_PLAYER_ID . '"></div>';

  if (arg(2) == 'embed') {
    $cover_image_preset = 'media-audio-cover-embed';
  }
  else {
    $cover_image_preset = 'media-audio-cover';
  }

  // Theming of Prolocation items
  if($wrapper instanceof MediaBVDStreamWrapper || $wrapper instanceof MediaProlocationStreamWrapper) {
    $vars['cover_image'] = theme('image_style', array('style_name' => $cover_image_preset, 'path' => $wrapper->getLocalThumbnailPath()));

    flowplayer_add('#' . $player_id, array(
      'playlist'  => array(
        array(
           // Replace single quote in URL for IE compatibility
          'url'         => str_replace("'", '%2527', $wrapper->getExternalPath()),
          'provider'    => 'rtmp',
          'autoPlay'    => false,
          'onStart'	    => 'Drupal.BGMediaPlayerPlayHandler',
          'onResume'    => 'Drupal.BGMediaPlayerPlayHandler',
          'onStop'	    => 'Drupal.BGMediaPlayerStopHandler',
          'onPause'     => 'Drupal.BGMediaPlayerStopHandler',
        ),
      ),
      'play'      => NULL,
      'plugins'   => array(
        'rtmp'  => array(
          'url'               => 'flowplayer.rtmp.swf',
          'netConnectionUrl'  => $wrapper->getBaseUrl(),
        ),
        'controls'  => array(
          'height'     => 30,
          'autoHide'   => FALSE,
          'fullscreen' => FALSE,
        ),
      ),
    ));
  }
  else {
    $icon_path = str_replace('sites/default/files/', '', file_icon_path((object)$vars['file'], 'sites/default/files'));

    $vars['cover_image'] = theme('image_style', array('style_name' => $cover_image_preset, 'path' => $icon_path));

    flowplayer_add('#' . $player_id, array(
      'playlist'  => array(
        array(
          'url'       => base_path() . $wrapper->getDirectoryPath() . '/' . $vars['file']->filename,
          'autoPlay'  => false,
          'onStart'	=> 'Drupal.BGMediaPlayerPlayHandler',
          'onResume'  => 'Drupal.BGMediaPlayerPlayHandler',
          'onStop'	=> 'Drupal.BGMediaPlayerStopHandler',
          'onPause'   => 'Drupal.BGMediaPlayerStopHandler',
        ),media_variable_get('icon_set')
      ),
      'play'      => NULL,
      'plugins'   => array(
        'audio'  => array(
          'url'               => 'flowplayer.audio.swf',
        ),
        'controls'  => array(
          'height'     => 30,
          'autoHide'   => FALSE,
          'fullscreen' => FALSE,
        ),
      ),
    ));
  }
}

/**
 * Preprocess function for theme('bg_media_image').
 */
function bg_media_preprocess_bg_media_image(&$vars) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($vars['file']->uri);

  if($wrapper instanceof BGMediaStreamWrapper) {
    $vars['image_uri'] = $wrapper->getLocalThumbnailPath();
  }
  else if($wrapper instanceof DrupalPublicStreamWrapper) {
    $vars['image_uri'] = $wrapper->getUri();
  }
}

/**
 * Theme function for theme('bg_media_audio').
 */
function theme_bg_media_image(&$vars) {
  return theme('image_style', array(
    'style_name'  => 'media',
    'path'        => $vars['image_uri'],
  ));
}

function bg_media_load_videojs() {
  static $loaded;

  if(!isset($loaded)) {
    $loaded = false;
  }
  else if($loaded) {
    return;
  }

  $videojs_file_types = videojs_add(FALSE);
  foreach($videojs_file_types as $type => $file_type) {
    $function = "drupal_add_$type";

    foreach($file_type as $file) {
      // Filter out module files
      if(is_string($file['data']) && strpos($file['data'], 'modules/videojs') !== FALSE) {
        continue;
      }

      $file['type'] = isset($file['type']) ? $file['type'] : NULL;
      $function($file['data'], $file['type']);
    }
  }

  $loaded = TRUE;
}

function bg_media_get_videos($candidates, &$videos, &$fallback_video_url) {
  $codecs = array(
    'video/mp4'                 => "video/mp4; codecs='avc1.42E01E, mp4a.40.2'",
    'video/webm'                => 'video/webm; codec="vp8, vorbis"',
    'application/octet-stream'  => 'video/webm; codec="vp8, vorbis"',
    'video/ogg'                 => 'video/ogg; codec="theora, vorbis"',
    'application/ogg'           => 'video/ogg; codec="theora, vorbis"',
    'video/quicktime'           => 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"'
  );

  foreach($candidates as &$video_candidate) {
    $video_candidate = get_object_vars(file_uri_to_object($video_candidate));

    // Skip unplayable items.
    if (!isset($codecs[$video_candidate['filemime']])) {
      continue;
    }

    $video_candidate['video_url']   = file_create_url($video_candidate['uri']);
    $video_candidate['video_type']  = $codecs[$video_candidate['filemime']];

    // Set fallback video
    if($video_candidate['filemime'] == 'video/mp4' && empty($fallback_video_url)) {
      $fallback_video_url = $video_candidate['video_url'];
    }

    $videos[] = $video_candidate;
  }
}
