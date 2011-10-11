(function($) {
  Drupal.BGMediaPlayerLoadHandler = function() {
    if(typeof(VideoJS) == 'function') {
      var video = document.createElement("video");
      var videotagsupport = !!video.canPlayType;

      // Check if video tag is supported
      if(videotagsupport) {
        VideoJS.setup();
      }

      // RVW: Removed $('#media-video').bind('play', function(){Drupal.BGMediaPlayerPlayHandler();});
      // RVW: Removed $('#media-video').bind('pause', function(){Drupal.BGMediaPlayerStopHandler();});
    }
  }

  Drupal.BGMediaPlayerPlayHandler = function() {
    // RVW: Removed $('.video-titlebar, .group_verbreding').fadeOut('fast');
  }

  Drupal.BGMediaPlayerStopHandler = function() {
    // RVW: Removed $('.video-titlebar, .group_verbreding').fadeIn('fast');
  }

  Drupal.behaviors.bgDossierMediaBehavior = {
    attach: function (context, settings) {
      var mediaPreviews = $('.dossier-element-media a.use-ajax:not(.main-media-player)');
      if($('body').hasClass('node-type-dossier') && mediaPreviews.length > 0) {
        mediaPreviews.each(function(){
          linkInfo = $(this).attr('rel').split('|');

          // Set new href
          $(this).attr('href', '/ajax/switch-media/'
              + Drupal.settings.bg_reference.dossierNid + '/'
              + Drupal.settings.bg_reference.mainMediaNid + '/'
              + linkInfo[0] + '/'
              + linkInfo[1])
            // Unbind existing handlers
            .unbind('click')
            // Force rebinding of AJAX handlers
            .removeClass('ajax-processed');
        });

        $('.main-media-player').bind('mouseover', function(){
          $(this).addClass('dossier-element-focus');
        });

        $('.main-media-player').bind('mouseout', function(){
          $(this).removeClass('dossier-element-focus');
        });

        // Force reattachment of AJAX behavior
        Drupal.behaviors.AJAX.attach(context, settings);
      }
    }
  }

  $.fn.bgDossierMediaChange = function(newThumb) {
    // Build field groups
    Drupal.FieldGroup.Effects.processDiv.execute($('.main-media-player'));

    // Display transition effect
    $(newThumb).effect('transfer', {to: $('.main-media-player')}, 1000);
    // Parse FBML
    FB.XFBML.parse();
  }
})(jQuery);
