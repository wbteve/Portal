(function($) {
  Drupal.behaviors.xmlfeeditems = {
    attach: function(context) {
      $('.xml-feed-items li li').hover(function(){
        $(this).find('ul').show();
      },function(){
        $(this).find('ul').hide();
      }
      );
    }
  }
})(jQuery);
