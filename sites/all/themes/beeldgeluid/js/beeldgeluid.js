(function ($) {

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
      $('.group-verbreding').addClass('group-verbreding-closed').click(bgToggleVerbreding);
      $(".verbreding-item-inner").click(function(event){
        event.stopPropagation();
      });
      $(".verbreding-embed-code").focus(function(event){
        this.select();
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

  function bgCollapseComments() {
    $('#comments h2.title').each(function() {
      $(this).addClass('closed');
      $(this).nextAll('.comment-list, form').filter(':first').hide();
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

  function bgToggleVerbreding(event) {
    var $this = $(this);
    if ($this.hasClass('group-verbreding-closed')) {
      $(this).animate({'width': '280px'}).removeClass('group-verbreding-closed');
    }
    else {
      $(this).animate({'width': '60px'}).addClass('group-verbreding-closed');
    }
  }
}(jQuery));
