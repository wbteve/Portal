<?php if(!empty($videos)) : ?>
  <div class="video-js-box <?php print variable_get('videojs_skin', 'default'); ?>" id="media">
    <video id="media-video" class="video-js" width="<?php echo $width; ?>" height="<?php echo $height; ?>" controls="controls" preload="auto" poster="<?php echo file_create_url($thumbnail['uri']); ?>">
      <?php foreach ($videos as $video): ?>
        <source src="<?php echo $video['video_url']; ?>" type="<?php echo $video['video_type']; ?>" />
      <?php endforeach; ?>

      <?php echo $flash_video; ?>
    </video>
  </div>
<?php endif; ?>
