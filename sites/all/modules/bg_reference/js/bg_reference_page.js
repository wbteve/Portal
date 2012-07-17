(function($) {
  Drupal.behaviors.xmlfeeditems = {
    attach: function(context) {
      $('.xml-feed-items li li').hover(function(e){
        $(this).find('ul').show();
        $(this).find('ul').css('top', e.pageY).css('left', e.pageX + 50);
      },function(){
        $(this).find('ul').hide();
      }
      );
    }
  }
})(jQuery);
