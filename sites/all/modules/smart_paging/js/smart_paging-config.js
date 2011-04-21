// $Id: smart_paging-admin.js,v 1.1 2010/09/18 04:00:30 arpeggio Exp $

(function ($) {
  Drupal.behaviors.smartPagingConfig = {
    attach: function (context, settings) {
      $(':input[name="' + settings.smart_paging.filter_html_name + '"]', context).bind('click', function (event) {
        event.stopPropagation();
        $(this, context).bind('change', function (e) {
          var show_config = false;
          for (var filter in settings.smart_paging.smart_paging_filter) {
            if (filter == $(this).attr('value')) {
              show_config = true;
              break;
            }
          }
          if (show_config) {
            $('fieldset.smart-paging-settings', context).removeClass('element-invisible');
          }
          else {
            $('fieldset.smart-paging-settings', context).addClass('element-invisible');
          }
        }); 
      });      
    }
  };
})(jQuery);