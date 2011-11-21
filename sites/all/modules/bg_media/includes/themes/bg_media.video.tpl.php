<?php if(isset($video)) : ?>
  <?php echo $video; ?>
<?php elseif(!empty($videos)) : ?>
  <div class="video-js-box <?php print variable_get('videojs_skin', 'default'); ?>">
    <video class="video-js" width="<?php echo $width; ?>" height="<?php echo $height; ?>" controls="controls" preload="auto" poster="<?php echo $thumbnail; ?>">
      <?php foreach ($videos as $video): ?>
        <source src="<?php echo $video['video_url']; ?>" type="<?php echo $video['video_type']; ?>" />
      <?php endforeach; ?>

      <?php echo $flash_video; ?>
    </video>
  </div>
<?php endif; ?>
