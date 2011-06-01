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
    var index = this.elements.push(element);

    // Add to frame
    var $item = $('<div />')
      .attr("id", "dossiergrid-item-" + index)
      .addClass("dossiergrid-item")
      .css({"width": element.x_size * this.zoom, 
            "height": element.y_size * this.zoom, 
            "background-color": element.bgcolor})
      .draggable({"containment": "parent",
                  "grid": [this.gridstep * this.zoom, this.gridstep * this.zoom],
                  "snap": true})
      .html(element.html);

    this.$frame.append($item);
  }

  alert('foo');

  // Remove later
  var DG = new DossierGrid("#gridui-frame", 50, .5);
  DG.add_media({x_size: 200, y_size: 200, bgcolor: "#0d0", image: "", html: ""});
  DG.add_media({x_size: 300, y_size: 200, bgcolor: "#00d", image: "", html: ""});
  DG.add_media({x_size: 100, y_size: 200, bgcolor: "#0dd", image: "", html: ""});
  DG.add_media({x_size: 400, y_size: 200, bgcolor: "#dd0", image: "", html: ""});
  DG.add_media({x_size: 200, y_size: 400, bgcolor: "#d0d", image: "", html: ""});

  alert('hi');
}(jQuery))
