<!-- Begin VideoJS -->
<?php dsm($items); if (count($items) > 0): ?>
  <div class="video-js-box <?php print variable_get('videojs_skin', 'default') ?>" id="media">
    <video id="media-video" class="video-js" width="480" height="360" controls="controls" preload="auto" poster="<?php print($poster) ?>">
      <?php foreach ($items as $item): ?>
        <?php $filepath = file_create_url($item['uri']); ?>
        <source src="<?php print(file_create_url($item['uri'])) ?>" type="<?php print($item['videotype']) ?>" />
      <?php endforeach; ?>
    </video>
  </div>
<?php endif; ?>
<!-- End VideoJS -->

