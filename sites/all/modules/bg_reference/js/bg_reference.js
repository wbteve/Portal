(function($) {
  Drupal.behaviors.bgReference = {
    attach: function(context) {
      
    }
  }
  
  if (!!Drupal.jsAC) { // this condition tests if autocomplete has been enabled
    /**
     * Hides the autocomplete suggestions.
     */
    Drupal.jsAC.prototype.hidePopup = function (keycode) {
      // Select item if the right key or mousebutton was pressed.
      if (this.selected && ((keycode && keycode != 46 && keycode != 8 && keycode != 27) || !keycode)) {
        this.input.value = $(this.selected).data('autocompleteValue');
        $('.bg_reference_content', $(this.input).closest('fieldset')).val( $(this.selected).data('autocompleteValue') );
      }
      // Hide popup.
      var popup = this.popup;
      if (popup) {
        this.popup = null;
        $(popup).fadeOut('fast', function () { $(popup).remove(); });
      }
      this.selected = false;
      $(this.ariaLive).empty();
    };
  }
})(jQuery);