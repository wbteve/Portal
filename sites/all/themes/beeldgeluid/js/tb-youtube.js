//
//To Track Thy Youtube Upon Google Analytics
//Regardless the number of Players upon thy stage
//Revised and Revisioned to Version 2
//Within the March of Two Thousand and Thirteen
//
//Performed by LunaMetrics http://www.lunametrics.com  
//and Sayf Sharif @sayfsharif
//
//Who beg thy forgiveness for the lack of the regular expression
//Tis code not the most triumphant
//Nor that of most efficiency
//but neither is the field of poppies
//stretch'd ere the distance
//it matches its purpose
//
//
//CURTAIN
//
//Forsooth here doth we instantiate thy youtube player api 
//as it was written by the Google
var tag = document.createElement('script');
tag.src = "//www.youtube.com/iframe_api";
var firstScriptTag = document.getElementsByTagName('script')[0];
firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);
//Then as a drop of rain we create two heavenly arrays
//who may hold in thy endless bossom our value
//necessary they may be not, but what is love
//but the ample clevage of an array
var videoArray = new Array();
var playerArray = new Array();
//twixt the jquery we fly to watch as the eagle does the mouse
//for the wanderous devil known as the iframe
(function($) {
	function trackYouTube()
	{
		//What am i, but nothing?
		var i = 0;
		//Harken to the iframes of the page
		//thy loathesome demon gallavanting upon
		//our innocent sweet html
		jQuery('iframe').each(function() {
			//but what is this?
			//an iframe! Avast!
			var video = $(this);
			//it has a source!
			//Lo we can see it's innards
			//as Han was wont to slice the tauntaun
			var vidSrc = "";
			vidSrc = video.attr('src');
			//We shall check the source
			//lo ere the response incorrect
			//we shall ignore it.
			//Once we did this brutally
			//with the ham fist of strange logic
			//until Nicole did deliver this
			//Upon the comments
			//http://www.lunametrics.com/blog/2012/10/22/automatically-track-youtube-videos-events-google-analytics/
			//the wonders of Reg Ex
			var regex = /https?\:\/\/www\.youtube\.com\/embed\/([\w-]{11})(?:\?.*)?/;
			var matches = vidSrc.match(regex);
			//Should the former reg provide a match
			//it shall appear in an array of matches
			if(matches && matches.length > 1){
					//we now place the beating heart of the youtube id
					//in our first heavenly array
					videoArray[i] = matches[1];
					//and then mark the vile iframe beast
					//with the id of this video so that all
					//may know it, and reference it
					$(this).attr('id', matches[1]);
					//And for this, I am no longer nothing, I am more
					i++;			
			}
		});	
	}
	//The beginning!
	//Sometimes buried deep within the story
	//but here we start our tale in earnest
	$(document).ready(function() {
		//here begins our quest to find the foul iframes
		//so infected with the source of the youtube
		trackYouTube();
	});
})(jQuery);
//When the API of YouTube doth load, it will call
//as if by magic
//this function or code. 
//be ready
function onYouTubeIframeAPIReady() {
	//Now the battle is engaged
	//we sweep through our array and reference the
	//beating hearts of the youtube id's
	//We then create a new holy object into our
	//second array, by referencing each beating
	//youtube id heart, and tell it wence it
	//shall act upon events
	for (var i = 0; i < videoArray.length; i++) {
		playerArray[i] = new YT.Player(videoArray[i], {
			events: {
			'onReady': onPlayerReady,
			'onStateChange': onPlayerStateChange
			}
		});		
	}
}
//Should one wish our monstrous video to play upon load
//we could set that here. But for us. We shall let it
//sleep. Sleep video. Await thy time.
function onPlayerReady(event) {
	//event.target.playVideo();
}
//And lo did Chris Green say
//upon the blog comments
//http://www.lunametrics.com/blog/2012/10/22/automatically-track-youtube-videos-events-google-analytics/
//Why not a pause flag
//one to prevent the terrors of the spammy
//pause events when a visitor
//doth drag the slide bar
//cross't thy player
//and all said huzzah
//let us start by setting his flag to false
//so that we know it is not true
var _pauseFlag = false;
//When our caged monster wishes to act
//we are ready to hold it's chains
//and enslave it to our will.
function onPlayerStateChange(event) { 
	//it tries to trick us with a number one greater than
	//that of our arrays. But we outsmart it.
	//with math.
	videoarraynum = event.target.id - 1;
	//Should the video rear it's head
	if (event.data ==YT.PlayerState.PLAYING){
		_gaq.push(['_trackEvent', 'Videos', 'Play', videoArray[videoarraynum] ]); 
		//thy video plays
		//reaffirm the pausal beast is not with us
		_pauseFlag = false;
	} 
	//should the video tire out and cease
	if (event.data ==YT.PlayerState.ENDED){
		_gaq.push(['_trackEvent', 'Videos', 'Watch to End', videoArray[videoarraynum] ]); 
	} 
	//and should we tell it to halt, cease, heal.
	//confirm the pause has but one head and it flies not its flag
	//lo the pause event will spawn a many headed monster
	//with events overflowing
	if (event.data ==YT.PlayerState.PAUSED && _pauseFlag == false){
		_gaq.push(['_trackEvent', 'Videos', 'Pause', videoArray[videoarraynum] ]); 
		//tell the monster it may have
		//but one head
		_pauseFlag = true;
	}
	//and should the monster think, before it doth play
	//after we command it to move
	if (event.data ==YT.PlayerState.BUFFERING){
		_gaq.push(['_trackEvent', 'Videos', 'Buffering', videoArray[videoarraynum] ]); 
	}
	//and should it cue
	//for why not track this as well.
	if (event.data ==YT.PlayerState.CUED){
		_gaq.push(['_trackEvent', 'Videos', 'Cueing', videoArray[videoarraynum] ]); 
	} 
} 
//fin
