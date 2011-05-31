<?php

/**
 * @file
 * Styles class implementation for Media: YouTube.
 */

class MediaYouTubeStyles extends FileStyles {
  public $autoplay;
  public $fullscreen;

  function get_autoplay() {
    return $this->get('autoplay');
  }
  function set_autoplay($value) {
    return $this->set('autoplay', $value);
  }
  function get_fullscreen() {
    return $this->get('fullscreen');
  }
  function set_fullscreen($value) {
    return $this->set('fullscreen', $value);
  }

  function get_image_uri() {
    if ($image_uri = $this->get('image_uri')) {
      return $image_uri;
    }
    $object = $this->get_object();
    if ($object->uri) {
      $wrapper = file_stream_wrapper_get_instance_by_uri($object->uri);
      return $wrapper->getLocalThumbnailPath();
    }
  }
  function video($effect) {
    $variables = array(
      'uri' => $this->get_uri(),
      'width' => $this->get_width(),
      'height' => $this->get_height(),
      'autoplay' => $this->get_autoplay(),
      'fullscreen' => $this->get_fullscreen(),
    );
    $this->set_output(theme('media_youtube_video', $variables));
  }
}
