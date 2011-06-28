(function ($) {
  $(function() {
    $('#comments h2.title').each(function() {
      $(this).addClass('closed');
      $(this).nextAll('div,form').filter(":first").hide();
      $(this).click(function() {
        $(this).toggleClass('closed');
        $(this).nextAll('div,form').filter(":first").slideToggle('fast');
      });
    });
    
    if ($('.node-type-dossier #collapse-content .field-name-body .field-item').html().length != 0) {
      $('.node-type-dossier #collapse-content .field-name-body').hide();
      $('.node-type-dossier h1#page-title').addClass('closed');
      $('.node-type-dossier h1#page-title').click(function() {
        $(this).toggleClass('closed').toggleClass('open');
        $('.node-type-dossier #collapse-content .field-name-body').slideToggle('fast');
      });
    }
    
    /*
    if($('.node-type-dossier').length != 0) {
      var totalHeight = 0;
      var totalWidth = 0;
      $('.dossier-element').each(function() {
        totalHeight = $(this).height() + $(this).position().top > totalHeight ? $(this).height() + $(this).position().top : totalHeight;
        totalWidth = $(this).width() + $(this).position().left > totalWidth ? $(this).width() + $(this).position().left : totalWidth;
      });
      var marginLeft = -totalWidth/2;
      $('.field-name-field-referenced-content').height(totalHeight);
      $('.field-name-field-referenced-content').width(totalWidth);
      if($('body').width() < totalWidth) {
        $('body').width(totalWidth)
        $(document).scrollLeft((totalWidth-$(window).width())/2);

      }
      $('.field-name-field-referenced-content').css('marginLeft', marginLeft);
    }
    */
  });
}(jQuery));
