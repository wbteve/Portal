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

  Drupal.behaviors.bgDossierMediaBehavior = {
    attach: function (context, settings) {
      $('.dossier-element-media a.use-ajax:not(.main-media-player)').each(function(){
        linkInfo = $(this).attr('rel').split('|');

        // Set new href
        $(this).attr('href', 'ajax/switch-media/'
            + Drupal.settings.bg_reference.dossierNid + '/'
            + Drupal.settings.bg_reference.mainMediaNid + '/'
            + linkInfo[0] + '/'
            + linkInfo[1])
          .unbind('click')
          .removeClass('ajax-processed');
      });
    }
  }

  $.fn.bgDossierMediaChange = function(newThumb) {

  }
})(jQuery);
