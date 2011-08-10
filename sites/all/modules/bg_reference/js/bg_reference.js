(function($) {
  Drupal.behaviors.bgReference = {
    attach: function(context) {
      // Handle input from text input/textarea
      $('.input-wrapper input[type="text"]', context)
        .bind('blur', handleInput)
        .trigger('blur');

      // Handle input from tinyMCE
      tinyMCE.onAddEditor.add(function(sender, editor) {
        $('.input-wrapper textarea', context).each(function(){
          var _self = this;

          if(editor.editorId == $(_self).attr('id')) {
            // In text mode
            $(_self).change(function() {
              console.log($(_self).val());
              $('input.bg_reference_content', $(_self).closest('fieldset')).val( $(_self).val() );
            });

            // In WYSIWYG mode
            editor.onSaveContent.add(function(editor) {
                $('input.bg_reference_content', $(_self).closest('fieldset')).val( editor.getContent() );
            });
          }
        });
      });

      // Apply color to color_back select options.
      $('select.bg_reference_color_back').each(function(index) {
        $(this).css({'color': '#fff', 'background-color': $(this).val()});
      }).change(function(event){
        $(this).css('background-color', $(this).val());
      }).find('option').css('color', '#fff').each(function(index) {
        $(this).css('background-color', $(this).val());
      });
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

  function handleInput(evt) {
    $('input.bg_reference_content', $(this).closest('fieldset')).val( $(this).val() );
  }
})(jQuery);
