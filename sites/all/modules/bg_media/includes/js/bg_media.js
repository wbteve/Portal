(function($) {
  Drupal.BGMediaPlayerLoadHandler = function() {
    if(typeof(VideoJS) == 'function') {
      var player = VideoJS.setup();
      $('#media-video').bind('play', function(){Drupal.BGMediaPlayerPlayHandler();});
      $('#media-video').bind('pause', function(){Drupal.BGMediaPlayerStopHandler();});
    }
  }

  Drupal.BGMediaPlayerPlayHandler = function() {
    $('.video-titlebar, .group_verbreding').fadeOut('fast');
  }

  Drupal.BGMediaPlayerStopHandler = function() {
    $('.video-titlebar, .group_verbreding').fadeIn('fast');
  }
})(jQuery);
