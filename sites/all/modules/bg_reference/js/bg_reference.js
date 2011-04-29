Drupal.behaviors.bg_reference = function (context) {
  if (Drupal.jsAC) {
    /**
     * Hides the autocomplete suggestions
     */
    Drupal.jsAC.prototype.hidePopup = function (keycode) {
      // Select item if the right key or mousebutton was pressed
      if (this.selected && ((keycode && keycode != 46 && keycode != 8 && keycode != 27) || !keycode)) {
        this.input.value = this.selected.autocompleteValue;
      }
      // Hide popup
      var popup = this.popup;
      if (popup) {
        this.popup = null;
        $(popup).fadeOut('fast', function() {
          $(popup).remove();
        });
        // Add-on for OpenLayer Geocoder module
        /*if ($(this.input).attr('geoautocomplete')) {
          geocoder = new Drupal.Geocoder(this);
          geocoder.process(this.input.value);
        }*/
      }
      this.selected = false;
    };
  }
};