(function($) {
  function DossierGrid(frame, gridstep, zoom) {
    this.$frame = $(frame);
    this.elements = [];
    this.gridstep = {'x': Drupal.settings.bg_reference.unitSizeWidth,
                     'y': Drupal.settings.bg_reference.unitSizeHeight};
    this.zoom = zoom;
  }

  DossierGrid.prototype.add_media = function(element) {

    var _this = this;
    var index = this.elements.push(element);
    var lang  = Drupal.settings.gridui.lang;

    // Add to frame
    var $item = $('<div />')
      .attr("id", "dossiergrid-item-" + index)
      .addClass("dossiergrid-item")
      .css({"width": element.x_size * this.gridstep.x * this.zoom -2,
            "height": element.y_size * this.gridstep.y * this.zoom -2,
            "border": "1px solid #fff",
            "background-color": element.bgcolor})
      .draggable({"containment": "parent",
                  "grid": [this.gridstep.x * this.zoom, this.gridstep.y * this.zoom],
                  "snap": true,
                  "stop": function(event, ui) {
                            // Use the starting position of this grid node to calculate the new position (according to the grid + scale)
                            var id = "edit-field-referenced-content-" + lang + "-" + (index-1);

                            var x = ui.position.left,
                                y = ui.position.top;

                            // Translate to grid units
                            var x_gu = Math.round(x / _this.gridstep.x / _this.zoom),
                                y_gu = Math.round(y / _this.gridstep.y / _this.zoom);

                            // Set values in form fields
                            $("#" + id + "-pos-x").val(x_gu);
                            $("#" + id + "-pos-y").val(y_gu);

                          }})
      .css({"top": element.y_pos * this.gridstep.y * this.zoom,
            "left": element.x_pos * this.gridstep.x * this.zoom,
            "position": "absolute"})
      .html(element.html);

    $("#dossiergrid-item-" + index).remove();

    this.$frame.append($item);
  }


  Drupal.behaviors.gridui_reference = {
    attach: function(context) {
      var DG = new DossierGrid("#gridui-frame", 50, .5);

      $("tr.draggable").each(function() {
        if(!$(this).hasClass("gridui-processed")) {
          $(this).addClass("gridui-processed");

          var newItem = $(this).find('.bg_reference_content').val() == '';
          if (newItem) {
            $(this).addClass("gridui-new-item");
          }

          var $fieldset = $("fieldset", $(this));
          var id = $fieldset.attr("id").split(/-/);
          var id = id.slice(0, 6).join('-');

          var index = parseInt(id.split(/-/)[5]);
          var lang = id.split(/-/)[4];

          var element = {x_size: $(".bg_reference_width", $fieldset).val(),
                         y_size: $(".bg_reference_height", $fieldset).val(),
                         x_pos: $(".bg_reference_pos_x", $fieldset).val(),
                         y_pos: $(".bg_reference_pos_y", $fieldset).val(),
                         bgcolor: newItem ? "#666": "#000",
                         image: "",
                         html: (index+1)};

          DG.add_media(element);

          var $container = $($("td", $(this))[1]);

          var newItemText = newItem ? ' <span class="new-item-text">(' + Drupal.t('New') + ')</span>' : '';
          var $title = $("<h2>Item #" + (index+1) + newItemText + "</h2>")
                         .addClass("gridui-header")
                         .click(function() {
                           $fieldset.toggle();
                         });

          $container.prepend($title);

          $(".form-item-field-referenced-content-" + lang + "-" + index + "-pos-x").hide();
          $(".form-item-field-referenced-content-" + lang + "-" + index + "-pos-y").hide();

        }

      });

      $(".field-multiple-table + .clearfix").removeClass("clearfix");
    }
  }
}(jQuery));
