(function($) {
  var initializedVideoPlayer = false;

  Drupal.BGMediaPlayerLoadHandler = function() {
    if(typeof(VideoJS) == 'function' && (!Drupal.settings.bgMedia.initOnce || (Drupal.settings.bgMedia.initOnce && !initializedVideoPlayer))) {
      var video = document.createElement("video");
      var videotagsupport = !!video.canPlayType;

      // Check if video tag is supported
      if(videotagsupport) {
        VideoJS.autoSetup();
      }

      $('.video-js').each(function() {
        var player = VideoJS(this);
        player.addEvent('play', function() { Drupal.BGMediaPlayerPlayHandler.call(this); });
        player.addEvent('pause', function() { Drupal.BGMediaPlayerStopHandler.call(this); });
        player.addEvent('ended', function() { Drupal.BGMediaPlayerFinishHandler.call(this); });
      });
    }

    initializedVideoPlayer = true;
  }

  Drupal.BGMediaPlayerPlayHandler = function() {
    if (typeof this.getConfig == 'function') {
      trackVideoEvent('play', this.getConfig().title);
    }
    else {
      trackVideoEvent('play', $(this.tech.el).data('title'));
    }

    // RVW: Removed $('.video-titlebar, .group_verbreding').fadeOut('fast');
  }

  Drupal.BGMediaPlayerStopHandler = function() {
    if (typeof this.getConfig == 'function') {
      trackVideoEvent('stop', this.getConfig().title);
    }
    else {
      trackVideoEvent('stop', $(this.tech.el).data('title'));
    }

    // RVW: Removed $('.video-titlebar, .group_verbreding').fadeIn('fast');
  }

  Drupal.BGMediaPlayerFinishHandler = function() {
    if (typeof this.getConfig == 'function') {
      trackVideoEvent('volledig afgespeeld', this.getConfig().title);
    }
    else {
      trackVideoEvent('volledig afgespeeld', $(this.tech.el).data('title'));
    }
  }

  Drupal.behaviors.bgDossierMediaBehavior = {
    attach: function (context, settings) {
      if($('body').hasClass('node-type-dossier')) {
        var mediaPreviews = $('.dossier-element-media a.use-ajax:not(.main-media-player)');
        if (mediaPreviews.length > 0) {
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
        }
/*
        $('.main-media-player').bind('mouseover', function(){
          $(this).addClass('dossier-element-focus');
        });

        $('.main-media-player').bind('mouseout', function(){
          $(this).removeClass('dossier-element-focus');
        });
*/
        $('.main-media-player .video-titlebar').append($('<div class="media-verdieping-open"></div>'));
        $('.media-verdieping-open').click(function(){
          $(this).parents('.main-media-player').toggleClass('dossier-element-focus');
        });
        
        // Force reattachment of AJAX behavior
        if (typeof(Drupal.behaviors.AJAX) == 'object') {
          Drupal.behaviors.AJAX.attach(context, settings);
        }
      }
    }
  }

  function trackVideoEvent(event, title) {
    _gaq.push([ '_trackEvent', 'videos', event, title ]);
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
