var bfimg = new Image();
var blfimg = new Image();
var gfimg = new Image();
var pfimg = new Image();
var rfimg = new Image();
var yfimg = new Image();

let fish1;
let fish2;
let fish3;
let fish4;




const clamp = (num, min, max) => Math.min(Math.max(num, min), max);

function init() {
	bfimg.src = './img/spritesheets/__cartoon_fish_06_black_swim.png';
	blfimg.src = './img/spritesheets/__cartoon_fish_06_blue_swim.png';
	gfimg.src = './img/spritesheets/__cartoon_fish_06_green_swim.png';
	pfimg.src = './img/spritesheets/__cartoon_fish_06_purple_swim.png';
	rfimg.src = './img/spritesheets/__cartoon_fish_06_red_swim.png';
	yfimg.src = './img/spritesheets/__cartoon_fish_06_yellow_swim.png';

	var canvas = document.getElementById("canvas");
	var ctx = canvas.getContext("2d");
	ctx.canvas.width = screen.width;
	ctx.canvas.height = screen.height;
	ctx.fillStyle = "#0C3BD8";//"#0C3BD8"
	ctx.fillRect(0, 0, canvas.width, canvas.height);

	fish1 = new Fish(ctx.canvas.width, ctx.canvas.height, 0);
	fish2 = new Fish(ctx.canvas.width, ctx.canvas.height, 100);
	fish3 = new Fish(ctx.canvas.width, ctx.canvas.height, 500);
	fish4 = new Fish(ctx.canvas.width, ctx.canvas.height, 1000);

	window.requestAnimationFrame(draw);
}

function draw() {
	var ctx = document.getElementById('canvas').getContext('2d');
	ctx.fillRect(0, 0, canvas.width, canvas.height);




	fish1.update()
	if (fish1.isDead()) {
		fish1.reset(ctx.canvas.width, ctx.canvas.height);
	}
	ctx.drawImage(fish1.img, fish1.sx, fish1.sy, 500, 325, fish1.posx, fish1.posy, fish1.width, fish1.height);

	fish2.update()
	if (fish2.isDead()) {
		fish2.reset(ctx.canvas.width, ctx.canvas.height);
	}
	ctx.drawImage(fish2.img, fish2.sx, fish2.sy, 500, 325, fish2.posx, fish2.posy, fish2.width, fish2.height);

	fish3.update()
	if (fish3.isDead()) {
		fish3.reset(ctx.canvas.width, ctx.canvas.height);
	}
	ctx.drawImage(fish3.img, fish3.sx, fish3.sy, 500, 325, fish3.posx, fish3.posy, fish3.width, fish3.height);

	fish4.update()
	if (fish4.isDead()) {
		fish4.reset(ctx.canvas.width, ctx.canvas.height);
	}
	ctx.drawImage(fish4.img, fish4.sx, fish4.sy, 500, 325, fish4.posx, fish4.posy, fish4.width, fish4.height);




	window.requestAnimationFrame(draw);
}

document.addEventListener("DOMContentLoaded", function () {
	const tween1 = KUTE.fromTo(
		'#blob1',
		{ path: '#blob1' },
		{ path: '#blob2' },
		{ repeat: 999, duration: 3000, yoyo: true }
	)
	const tween2 = KUTE.fromTo(
		'#blob3',
		{ path: '#blob3' },
		{ path: '#blob4' },
		{ repeat: 999, duration: 3000, yoyo: true }
	)
	const tween3 = KUTE.fromTo(
		'#blob5',
		{ path: '#blob5' },
		{ path: '#blob6' },
		{ repeat: 999, duration: 3000, yoyo: true }
	)
	const tween4 = KUTE.fromTo(
		'#blob7',
		{ path: '#blob7' },
		{ path: '#blob8' },
		{ repeat: 999, duration: 3000, yoyo: true }
	)
	tween1.start()
	tween2.start()
	tween3.start()
	tween4.start()
	init();
});


class Fish {
	constructor(posx, cheight, startframe) {
		//image width is 500 and height is 325
		this.posx = posx;
		this.posy = getRandomArbitrary(0, cheight - 250);
		this.framecount = 0;
		this.stepx = 0;
		this.stepy = 0;
		this.scale = clamp((Math.random() / 2), 0.1, 0.5);
		this.sx = this.stepx * 500;
		this.sy = this.stepy * 325;
		this.width = 500 * this.scale;
		this.height = 325 * this.scale;
		this.startframe = startframe;
		this.img = null;
		this.getimage();
	}

	update() {
		this.framecount++;
		if (this.framecount < this.startframe) {

			return;
		}

		if (this.framecount % 15 == 0) {
			this.stepx += 1;
			if (this.stepx > 3) {
				this.stepx = 0;
				this.stepy++;
				if (this.stepy > 2) {
					this.stepy = 0;
				}
			}

			this.sx = this.stepx * 500;
			this.sy = this.stepy * 325;
		}
		this.posx -= 1;


	}

	isDead() {
		return (this.posx < -500)
	}
	reset(posx, cheight) {
		this.posx = posx;
		this.scale = clamp((Math.random() / 2), 0.1, 0.5);
		this.posy = getRandomArbitrary(0, cheight - 250);
		this.framecount = 0;
		this.stepx = 0;
		this.stepy = 0;
		this.sx = this.stepx * 500;
		this.sy = this.stepy * 325;
		this.width = 500 * this.scale;
		this.height = 325 * this.scale;
		this.getimage();
	}
	getimage() {
		let randint = getRandomInt(0, 6);
		switch (randint) {
			case 0:
				this.img = bfimg;
				break;
			case 1:
				this.img = blfimg;
				break;
			case 2:
				this.img = gfimg;
				break;
			case 3:
				this.img = pfimg;
				break;
			case 4:
				this.img = rfimg;
				break;
			case 5:
				this.img = yfimg;
				break;
			default:
				this.img = bfimg;
				break;
		}
	}
}

function getRandomArbitrary(min, max) {
	return Math.random() * (max - min) + min;
}
function getRandomInt(min, max) {
	min = Math.ceil(min);
	max = Math.floor(max);
	return Math.floor(Math.random() * (max - min) + min); //The maximum is exclusive and the minimum is inclusive
}

window.addEventListener("resize", textChangeWidth);

function textChangeWidth(){
	sections = document.getElementsByClassName("section");
	let currentsize = window.innerWidth;

	console.log(currentsize);


	/*if(currentsize<1200){
		for (let i = 0; i < sections.length; i++) {
			const element = sections[i];
			element.style.margin = "100px 15vw";
		}
	}else if (currentsize<1000) {
		for (const secs in sections) {
			secs.style.margin = "100px 8vw";
		}
	}*/
}