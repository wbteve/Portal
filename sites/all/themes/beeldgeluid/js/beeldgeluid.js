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
  });
}(jQuery));
