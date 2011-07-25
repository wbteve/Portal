(function($) {
  Drupal.BGMediaPlayerLoadHandler = function() {
    if(typeof(VideoJS) == 'function') {
      var player = VideoJS.setup();
      $('#media-video').bind('play', function(){Drupal.BGMediaPlayerPlayHandler();});
      $('#media-video').bind('pause', function(){Drupal.BGMediaPlayerStopHandler();});
    }
  }

  Drupal.BGMediaPlayerPlayHandler = function() {
    $('.video-titlebar, .group_verbreding').fadeOut('fast');
  }

  Drupal.BGMediaPlayerStopHandler = function() {
    $('.video-titlebar, .group_verbreding').fadeIn('fast');
  }

  /* TO BE INCLUDED */

    $('.node-type-media').each(function() { 
      var $node = $(this); 
     
      $node.css({'cursor': 'pointer'}); 

      /** 
       * Swap the selected node with the current node. 
       **/ 
      $node.click(function() { 
        var $this = $(this); 
 
        // Some magic to get the class name we need 
        var node_class_parts = $this.attr('class').split(' '); 
        var node_class = node_class_parts[node_class_parts.length - 1]; 
 
        // Fetch data from Drupal settings 
        var node_data = Drupal.settings.bg_reference.media[node_class]; 
 
        /** 
         * The current media node (full), this needs to be here since we rename the class 
         * at the end of this function and it needs to be reloaded 
         **/ 
        var $current_node = $('#media').parents('.dossier-element'); 
        var current_class_parts = $current_node.attr('class').split(' '); 
        var current_class = current_class_parts[current_class_parts.length - 1]; 
        
        // Prepare a new DOM element to inject in place of the other player 
        // TODO: fetch through AJAX 
        /* 
        var $new_container = $('<div />'); 
        var $new_video = $('<video />') 
 
        $new_player.addClass('video-js-box'); 
        $new_player.addClass('videojs_skin'); // from node 
        $new_player.attr('id', 'media'); 
        
        $new_video.addClass('video-js'); 
        $new_video.attr('width', 'x'); // from node  
        $new_video.attr('height', 'x'); // from node  
        $new_video.attr('controls', 'controls'); 
        $new_video.attr('preload', 'auto'); 
        $new_video.attr('poster', 'x') // from node $thumbnail 
 
        // Append formats 
        // from node 
        for(var i = 0; i < videos.length; i++) { 
          var $new_source = $('<source />'); 
          $new_source.attr('src', 'src'); // from node (video_url) 
          $new_source.attr('type', 'type'); // from node (video_type) 
           
          $new_video.append($new_source); 
        } 
        */ 
 
        var POST = {'new_node_id': $node_id, 
                    'old_node_id': $current_id, 
                    'thumbnail_size': $gridsize}; 
 
        $('#media').empty(); 
 
        // Plug the new video in 
        // XXX 
 
        // Change the data of the clicked node by removing the class and adding a new one for the main item 
        // Swap the classes 
        $current_node.removeClass(current_class); 
        $current_node.addClass(node_class); 
 
        $this.removeClass(node_class); 
        $this.addClass(current_class); 
      });  
    }); 
   */
})(jQuery);
