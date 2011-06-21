(function($) {
  function DossierGrid(frame, gridstep, zoom) {
    this.$frame = $(frame);
    this.elements = [];
    this.gridstep = gridstep;
    this.zoom = zoom;

    // Set some properties on the element
    this.$frame.css({position: "relative"});
  }

  DossierGrid.prototype.add_media = function(element) {

    var _this = this;
    var index = this.elements.push(element);

    // Add to frame
    var $item = $('<div />')
      .attr("id", "dossiergrid-item-" + index)
      .addClass("dossiergrid-item")
      .css({"width": element.x_size * this.gridstep * this.zoom, 
            "height": element.y_size * this.gridstep * this.zoom, 
            "background-color": element.bgcolor,
            "position": "absolute"})
      .draggable({"containment": "parent",
                  "grid": [this.gridstep * this.zoom, this.gridstep * this.zoom],
                  "snap": true,
                  "stop": function(event, ui) {
                            // Use the starting position of this grid node to calculate the new position (according to the grid + scale)
                            var id = "edit-field-referenced-content-nl-" + (index-1);

                            var x = ui.position.left,
                                y = ui.position.top;

                            // Translate to grid units
                            var x_gu = Math.round(x / _this.gridstep / _this.zoom),
                                y_gu = Math.round(y / _this.gridstep / _this.zoom);

                            // Set values in form fields
                            $("#" + id + "-pos-x").val(x_gu);
                            $("#" + id + "-pos-y").val(y_gu);

                            console.log($("#" + id + "-pos-x").val());
                          }})
      .css({"top": element.y_pos * this.gridstep * this.zoom,
            "left": element.x_pos * this.gridstep * this.zoom})
      .html(element.html);

    this.$frame.append($item);
  }

  // Remove later
  var DG = new DossierGrid("#gridui-frame", 50, .5);

  $("#gridui-frame").empty();

  // XXX: For now we will fill this based upon the fields in the "Referenced Content"
  // This step both adds all the elements to the frame and hides certain fields of the referenced content
  // as these are now handled by the gridui frame, also do some styling
  $("tr", "tbody", "#field-referenced-content-values").each(function() {
    var $fieldset = $("fieldset", $(this));
    var id = $fieldset.attr("id");

    var index = parseInt(id.split(/-/)[id.split(/-/).length-1]);

    var element = {x_size: $("#" + id + "-width").val(),
                   y_size: $("#" +  id + "-height").val(),
                   x_pos: $("#" + id + "-pos-x").val(),
                   y_pos: $("#" + id + "-pos-y").val(),
                   bgcolor: "#000",
                   image: "",
                   html: '<div class="dossiergrid-item-inner">' + index + '</div>'};

    DG.add_media(element);

    var $container = $($("td", $(this))[1]);

    var $title = $("<h2>Item #" + index + "</h2>")
                   .addClass("gridui-header")
                   .click(function() {
                     $fieldset.toggle();
                   });

    $container.prepend($title);

    $(".form-item-field-referenced-content-nl-" + index + "-pos-x").hide();
    $(".form-item-field-referenced-content-nl-" + index + "-pos-y").hide();

  });


  $("#field-referenced-content-values + .clearfix").removeClass("clearfix");

}(jQuery));
