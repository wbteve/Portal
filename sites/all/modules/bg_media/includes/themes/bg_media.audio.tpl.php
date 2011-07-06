<?php
	/*
	 * TODO: Firefox supports html5 <audio> tags but not with mp3 content.
	 * This code needs fix/workaround.
	 * 
	 * TODO: Possibly floplayer supports audio formats.
	 */
?>

<audio width="<?php echo $width; ?>" height="<?php echo $height; ?>" controls="controls">
	<source src="<?php echo $url; ?>" type="<?php echo $mimetype; ?>" />
	<!--[if gt IE 6]>
    <object width="<?php echo $width; ?>" height="<?php echo $height; ?>" classid="clsid:02BF25D5-8C17-4B23-BC80-D3488ABDDC6B"><!
    [endif]--><!--[if !IE]><!-->
    <object width="<?php echo $width; ?>" height="<?php echo $height; ?>" type="<?php echo $mimetype; ?>" data="<?php echo $url; ?>">
    <!--<![endif]-->
    <param name="src" value="<?php echo $url; ?>" />
    <param name="showlogo" value="false" />
    <object width="<?php echo $width; ?>" height="<?php echo $height; ?>" type="application/x-shockwave-flash"
	data="/sites/all/libraries/jw-player/4/player.swf?file=<?php echo $url; ?>">
    	<param name="movie" value="/sites/all/libraries/jw-player/4/player.swf?file=<?php echo $url; ?>" />
    </object><!--[if gt IE 6]><!--></object><!--<![endif]-->
</audio>