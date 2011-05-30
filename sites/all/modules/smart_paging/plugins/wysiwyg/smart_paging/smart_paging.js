(function ($) {

Drupal.wysiwyg.plugins['smart_paging'] = {

  /**
   * Return whether the passed node belongs to this plugin.
   */
  isNode: function(node) {
    return ($(node).is('img.smart-paging'));
  },

  /**
   * Execute the button.
   */
  invoke: function(data, settings, instanceId) {
    if (data.format == 'html') {
      var content = this._getPlaceholder(settings);
    }
    else {
      var content = settings.smartPagingPagebreak;
    }
    if (typeof content != 'undefined') {
      Drupal.wysiwyg.instances[instanceId].insert(content);
    }
  },

  /**
   * Replace all <!--pagebreak--> tags with images.
   */
  attach: function(content, settings, instanceId) {
    var smartPagingPagebreak = settings.smartPagingPagebreak;
    // Some WYSIWYGs (CKEditor) will strip the slash from single tags:
    // <foo /> becomes <foo>
    var pagebreak = settings.smartPagingPagebreak.replace(/\/>/, '/?>').replace(/ /g, ' ?');
    var pattern = new RegExp(pagebreak, 'g');
    content = content.replace(pattern, this._getPlaceholder(settings));
    return content;
  },

  /**
   * Replace images with <!--pagebreak--> tags in content upon detaching editor.
   */
  detach: function(content, settings, instanceId) {
    var $content = $('<div>' + content + '</div>'); // No .outerHTML() in jQuery :(
    // #404532: document.createComment() required or IE will strip the comment.
    // #474908: IE 8 breaks when using jQuery methods to replace the elements.
    $.each($('img.smart-paging', $content), function (i, elem) {
      var pagebreak = $(settings.smartPagingPagebreak);
      elem.parentNode.insertBefore(pagebreak[0], elem);
      elem.parentNode.removeChild(elem);
    });
    return $content.html();
  },

  /**
   * Helper function to return a HTML placeholder.
   */
  _getPlaceholder: function (settings) {
    return '<img src="' + settings.path + '/images/spacer.gif" alt="&lt;--pagebreak-&gt;" title="&lt;--pagebreak--&gt;" class="smart-paging drupal-content" />';
  }
};

})(jQuery);
