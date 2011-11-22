(function ($) {

  var dossierWidth = 0;

  Drupal.behaviors.bgSearchResults = {
    attach: function (context, settings) {
      $('.search-result').css({'cursor': 'pointer'});

      $('.search-result').click(function(){
        window.location = $('a:first', $(this)).attr('href');
      });

      $('.page-search-site .search-result:has(img), .section-tags .search-result:has(img)').each(function(){
        (function (context) {
          // Initially hide information
          $('.info-box', context).hide();
          // Show information on mouseover
          context.bind('mouseover', function(){
            $('.info-box', context).show();
          });
          // Hide information on mouseout
          context.bind('mouseout', function(){
            $('.info-box', context).hide();
          });
        })($(this));
      });
    }
  };

  Drupal.behaviors.bgCollapseComments = {
    attach: function (context, settings) {
      bgCollapseComments();
    }
  };

  Drupal.behaviors.bgCollapseDossierBody = {
      attach: function (context, settings) {
        bgCollapseDossierBody();
      }
    };

  Drupal.behaviors.bgCollapseVerbreding = {
    attach: function (context, settings) {
      $('.group-verbreding', context).addClass('group-verbreding-closed').click(bgToggleVerbreding);

      $(".verbreding-item-inner", context).click(function(event){
        event.stopPropagation();
      });

      $(".verbreding-embed-code", context).focus(function(event){
        this.select();
      }).mouseup(function(e){ e.preventDefault(); });
;

      $('.embed-copy', context).click(function(event){
        event.preventDefault();
        $('.verbreding-embed-code').select();
      });
    }
  };

  function resizeDossier(dossier) {
    if($(window).width() < dossierWidth){
      $('body').width(dossierWidth);
    }else{
      $('body').width($(window).width());
    }
    var main_info_pos = $('#main-info').offset();
    var calc = main_info_pos.left - (Drupal.settings.bg_reference.unitSizeWidth*2);
    dossier.css('left', calc);
    dossier.css('position', 'absolute');
  }

  Drupal.behaviors.bgDossierPosition = {
    attach: function (context, settings) {
      var $dossierContent = $('body.node-type-dossier .dossier-content');
      var $dossierContainer = $('body.node-type-dossier #dossier-container');

      if ($dossierContainer.length > 0) {
        $dossierContent.height($dossierContainer.height());

        if(!dossierWidth){
          $(".dossier-element").each(function() {
            var width = $(this).position().left + $(this).width();
            if(dossierWidth < width) dossierWidth = width;
          });
        }
        resizeDossier($dossierContainer);
        $(window).resize(function() {
          resizeDossier($dossierContainer);
        });
      }
    }
  };

  Drupal.behaviors.bgImgHover = {
    attach: function (context, settings) {
      $('.dossier-element-1x1 .dossier-element-text-inner, .dossier-element-agenda-1x1 .node-agenda-view-mode-dossier').each(function() {
        $(this).hover(
          function() { $(this).addClass('dossier-text-hover'); },
          function() { $(this).removeClass('dossier-text-hover'); }
        );
      });
      $('.dossier-element .use-ajax').each(function() {
        $(this).hover(
          function() {
            // MouseIn
            $('.hover', this).remove();
            $('img', this).css('position', 'absolute');
            $(this).append(
              '<span class="hover" style="width: ' + ($('img', this).width()) + 'px; height: ' + ($('img', this).height()) + 'px"><span></span></span>'
            );
          },
          function() {
            $('.hover', this).remove();
          }
        );
      });
    }
  };

  Drupal.behaviors.searchLabel = {
    attach: function (context, settings) {
      // some variables.
      var $searchInput = $('#edit-search-block-form--2');
      var searchString = Drupal.t('search');

      // Event listeners.
      $searchInput.focus(function() {
        if ($searchInput.val() == searchString) {
          $searchInput.val('');
        }
      }).blur(function() {
        if ($searchInput.val() == '') {
          $searchInput.val(searchString);
        }
      });

      // Trigger blur event on page load.
      $searchInput.blur();

    }
  };

  Drupal.behaviors.searchLabel = {
    attach: function (context, settings) {
      $('.search-advanced-more-info').hide().before('<span class="search-advanced-more-trigger">&nbsp;</span>');
      $('.search-advanced-more-trigger').click(function(){
        $(this).toggleClass('active');
        $('.search-advanced-more-info').toggle();
      });
    }
  };

  function bgCollapseComments() {
    $('#comments h2.title').each(function() {
      $(this).click(function() {
        $(this).toggleClass('closed');
        $(this).nextAll('.comment-list, form').filter(':first').slideToggle('fast');
      });
    });
  }

  function bgCollapseDossierBody() {
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

  var bgVerbredingMouseLeave = function(event) {
      $(this)
      .animate(
        {'width': '60px'},
        200,
        function() {
          $(this).addClass('group-verbreding-closed');
          $(this).unbind('mouseleave', bgVerbredingMouseLeave);
          $('#block-system-main .dossier-element .node-media-view-mode-full').width(480);
        }
      );
  }

  function bgToggleVerbreding(event) {
    var $this = $(this);
    if ($this.hasClass('group-verbreding-closed')) {
      $('#block-system-main .dossier-element .node-media-view-mode-full').width(720);
      $(this)
      .width(60) // width isn't set on initial page load
      .removeClass('group-verbreding-closed') // set the background color
      .animate(
        {'width': '280px'},
        200,
        function() {
          $(this).bind('mouseleave', bgVerbredingMouseLeave);
        }
      );
    }
    else {
      // Called manually instead of bgVerbredingMouseLeave
      $(this)
      .animate(
        {'width': '60px'},
        200,
        function() {
          $(this).addClass('group-verbreding-closed');
          $(this).unbind('mouseleave', bgVerbredingMouseLeave);
        }
      );
    }
  }
}(jQuery));
