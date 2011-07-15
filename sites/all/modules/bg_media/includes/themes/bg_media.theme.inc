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
  $js_path = base_path() . drupal_get_path('module', 'bg_media') . '/includes/js';

  if($wrapper instanceof MediaYouTubeStreamWrapper) {
    $vars['video'] = theme('media_youtube_video', array('uri' => $vars['file']->uri, 'width' => $vars['width'], 'height' => $vars['height']));
  }
  if($wrapper instanceof MediaBVDStreamWrapper) {
    drupal_add_library('bg_media', 'swfobject');
    drupal_add_js($js_path . '/bg_media.js');

    $config = drupal_json_encode(
      array(
        'playlist'  => array(
          array(
            'url'     => file_create_url($wrapper->getLocalThumbnailPath()),
          ),
          array(
            'url'       => $wrapper->getExternalPath(),
            'provider'  => 'rtmp',
            'autoPlay'  => false,
          )
        ),
        'plugins'   => array(
          'rtmp'  => array(
            'url'               => $js_path . '/flowplayer/flowplayer.rtmp/flowplayer.rtmp-3.2.3.swf',
            'netConnectionUrl'  => $wrapper->getBaseUrl(),
          )
        )
      )
    );
    $config = str_replace("\"", "'", $config);

    drupal_add_js(array(
      'bgMedia' => array(
        'playerId'      => BG_MEDIA_FALLBACK_PLAYER_ID,
        'playerSwf'     => $js_path . '/flowplayer/flowplayer-3.2.7.swf',
        'playerWidth'   => $vars['width'],
        'playerHeight'  => $vars['height'],
        'playerFlVars'  => array(
          'config' => $config
        ),
      ),
    ), 'setting');

    $vars['video'] = '<div id="' . BG_MEDIA_FALLBACK_PLAYER_ID . '"></div>';
  }
  else if($wrapper instanceof BGMediaStreamWrapper) {
    $fallback_video_url = null;

    // Add the videojs library to the page.
    drupal_add_library('bg_media', 'videojs');
    drupal_add_library('bg_media', 'swfobject');
    drupal_add_js($js_path . '/bg_media.js');
    drupal_add_js(array(
      'bgMedia' => array(
        'playerId'      => BG_MEDIA_FALLBACK_PLAYER_ID,
        'playerSwf'     => $js_path . '/flowplayer/flowplayer-3.2.7.swf',
        'playerWidth'   => $vars['width'],
        'playerHeight'  => $vars['height'],
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
  }
}

/**
 * Preprocess function for theme('bg_media_audio').
 */
function bg_media_preprocess_bg_media_audio(&$vars) {
  $js_path      = base_path() . drupal_get_path('module', 'bg_media') . '/includes/js';

  $icon_path    = media_variable_get('icon_base_directory') . '/' . media_variable_get('icon_set');
  $poster_path  = base_path() . file_icon_path((object)$vars['file'], $icon_path);

  $wrapper      = file_stream_wrapper_get_instance_by_uri($vars['file']->uri);

  $vars['url'] = base_path() . $wrapper->getDirectoryPath() . '/' . $vars['file']->filename;
  $vars['mimetype'] = $vars['file']->filemime;
  $vars['flash_audio'] = '<div id="' . BG_MEDIA_FALLBACK_PLAYER_ID . '"></div>';

  drupal_add_library('bg_media', 'swfobject');
  drupal_add_js($js_path . '/bg_media.js');
  drupal_add_js(array(
    'bgMedia' => array(
      'playerId'      => BG_MEDIA_FALLBACK_PLAYER_ID,
      'playerSwf'     => $js_path . '/flowplayer/flowplayer-3.2.7.swf',
      'playerWidth'   => $vars['width'],
      'playerHeight'  => $vars['height'],
      'playerAttr'    => array(
        'class'         => 'flash-fallback'
      ),
    ),
  ), 'setting');

  $config = drupal_json_encode(
    array(
      'playlist'  => array(
        array(
          'url'       => $poster_path,
        ),
        array(
          'url'       => $vars['url'],
          'autoPlay'  => false,
        )
      ),
      'plugins'   => array(
        'audio'  => array(
          'url'               => $js_path . '/flowplayer/flowplayer.audio/flowplayer.audio-3.2.2.swf',
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
