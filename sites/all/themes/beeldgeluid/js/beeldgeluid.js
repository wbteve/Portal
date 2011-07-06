(function ($) {

  Drupal.behaviors.bgCollapseComments = {
    attach: function (context, settings) {
      $('#comments h2.title').each(function() {
        $(this).addClass('closed');
        $(this).nextAll('div,form').filter(":first").hide();
        $(this).click(function() {
          $(this).toggleClass('closed');
          $(this).nextAll('div,form').filter(":first").slideToggle('fast');
        });
      });
    }
  };

  Drupal.behaviors.bgCollapseDossierBody = {
      attach: function (context, settings) {
        if ($('.node-type-dossier #collapse-content .field-name-body .field-item').length != 0) {
          $('.node-type-dossier #collapse-content .field-name-body').hide();
          $('.node-type-dossier h1#page-title').addClass('closed').click(function() {
            var $this = $(this);
            $this.toggleClass('closed').toggleClass('open');
            $('.node-type-dossier #collapse-content .field-name-body').slideToggle('fast');
            if ($this.hasClass('closed')) {
              $('.field-name-field-referenced-content').fadeTo('fast', 1);
            }
            else {
              $('.field-name-field-referenced-content').fadeTo('fast', .3);      
            }
          });
        }
      }
    };

  Drupal.behaviors.bgCollapseVerbreding = {
    attach: function (context, settings) {
      $('.group-verbreding').addClass('group-verbreding-closed').click(function(event) {
        var $this = $(this);
        if ($this.hasClass('group-verbreding-closed')) {
          $(this).animate({'width': '280px'}).removeClass('group-verbreding-closed');
        }
        else {
          $(this).animate({'width': '60px'}).addClass('group-verbreding-closed');
        }
      });
    }
  };

  Drupal.behaviors.bgVideojsListner = {
    attach: function (context, settings) {
      if ($('.node-media-view-mode-full #media-video').length == 1) {
        setTimeout(function() {
          $mediaVideo = $('#media-video');
          $parentElement = $mediaVideo.parents('.dossier-element');
          var bgVideoPlayer = $mediaVideo[0].player;
          bgVideoPlayer.onPlay(function(){
            $('.video-titlebar').fadeOut('fast');
            $parentElement.addClass('dossier-element-focus');
            //$('.dossier-element').not($parentElement).fadeTo('fast', .3);
          });
          bgVideoPlayer.onPause(function(){
            $('.video-titlebar').fadeIn('fast');
            //$('.dossier-element').not($parentElement).fadeTo('fast', 1);
          });
        }, 2000);
      }
    }
  };

}(jQuery));