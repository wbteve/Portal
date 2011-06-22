(function($) {
  $(document).ready(function(){
    swfobject.embedSWF(Drupal.settings.bgMedia.playerSwf,
        Drupal.settings.bgMedia.playerId,
        Drupal.settings.bgMedia.playerWidth,
        Drupal.settings.bgMedia.playerHeight,
        "9.0.0",
        null,
        Drupal.settings.bgMedia.playerFlVars,
        Drupal.settings.bgMedia.playerParams,
        Drupal.settings.bgMedia.playerAttr,
        function(e) {
          if(typeof(VideoJS) != 'undefined') {
            VideoJS.setup();
          }
        }
      );
  });
})(jQuery);