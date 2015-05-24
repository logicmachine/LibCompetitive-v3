var settings = {
	"scale": 1.0,
	"offset_x": 500,
	"offset_y": 500,
	"hidden_layers": []
};
var mouse_state = {
	"dragging": false,
	"prev_x": 0,
	"prev_y": 0
};
var dump_data = [];
var layers = [];

var transform = function(coords){
	var scale = settings.scale;
	var offset_x = settings.offset_x;
	var offset_y = settings.offset_y;
	return coords.map(function(v, k){
		if(k % 2 == 0){
			return v * scale + offset_x;
		}else{
			return v * -scale + offset_y;
		}
	});
};

var apply_attribute = function(s, shape){
	s.attr({
		stroke: shape.stroke_color,
		strokeOpacity: shape.stroke_opacity,
		strokeStyle: shape.stroke_style,
		strokeWidth: shape.stroke_width,
		fill: shape.fill_color,
		fillOpacity: shape.fill_opacity
	});
};

var draw_grid = function(paper, step, attr){
	var scale = settings.scale;
	var width = $("#svg").innerWidth(), height= $("#svg").innerHeight();
	var draw_step = step * Math.pow(0.1, Math.floor(Math.log10(scale))) * scale;
	var offset_x = settings.offset_x;
	var offset_y = settings.offset_y;
	offset_x = offset_x - draw_step * Math.floor(offset_x / draw_step);
	offset_y = offset_y - draw_step * Math.floor(offset_y / draw_step);
	for(var p = offset_x; p < width; p += draw_step){
		paper.line(p, 0, p, height).attr(attr);
	}
	for(var p = offset_y; p < height; p += draw_step){
		paper.line(0, p, width, p).attr(attr);
	}
};

var render = function(data){
	var scale = settings.scale;
	var paper = Snap("#svg");
	paper.clear();
	layers = [];
	if($("#grid-check").is(":checked")){
		var style = {
			stroke: "#dedede",
			strokeWidth: 2.0
		};
		draw_grid(paper, 100.0, style);
	}
	if($("#subgrid-check").is(":checked")){
		var style = {
			stroke: "#dedede",
			strokeWidth: 0.5
		};
		draw_grid(paper, 10.0, style);
	}
	data.forEach(function(layer, index){
		if(settings.hidden_layers.indexOf(index) != -1){ return; }
		var group = paper.g();
		layer.shapes.forEach(function(shape){
			if(shape.kind == "point"){
				var c = transform(shape.coords);
				var s = paper.circle(c[0], c[1], 2.0);
				apply_attribute(s, shape);
				group.add(s);
			}else if(shape.kind == "line"){
			}else if(shape.kind == "segment"){
				var c = transform(shape.coords);
				var s = paper.line(c[0], c[1], c[2], c[3]);
				apply_attribute(s, shape);
				group.add(s);
			}else if(shape.kind == "circle"){
				var c = transform(shape.coords);
				var s = paper.circle(c[0], c[1], shape.radius * settings.scale);
				apply_attribute(s, shape);
				group.add(s);
			}else if(shape.kind == "polygon"){
				var s = paper.polygon(transform(shape.coords));
				apply_attribute(s, shape);
				group.add(s);
			}
		});
		layers.push(group);
	});
};

var make_list = function(data){
	var list = $("#layer-list");
	list.empty();
	data.forEach(function(layer, index){
		var checkbox = $('<input type="checkbox" />');
		if(settings.hidden_layers.indexOf(index) == -1){
			checkbox.attr({ checked: "checked" });
		}
		checkbox.change(function(){
			if($(this).is(":checked")){
				var p = settings.hidden_layers.indexOf(index);
				if(p != -1){ settings.hidden_layers.splice(p, 1); }
			}else{
				settings.hidden_layers.push(index);
			}
			render(data);
		});
		var label = $('<label>' + layer.name + '</label>');
		label.prepend(checkbox);
		var item = $('<div class="checkbox"></div>');
		item.prepend(label);
		list.prepend(item);
	});
};

var set_all_visibility = function(data, flag){
	var items = $("#layer-list").find("input");
	items.prop('checked', flag);
	settings.hidden_layers = [];
	if(!flag){
		data.forEach(function(layer, index){
			settings.hidden_layers.push(index);
		});
	}
	render(data);
}

var wheel = function(e){
	e.preventDefault();
	if(e.wheelDelta == 0){ return; }
	var scale = (e.wheelDelta < 0 ? (1.0 / 1.2) : 1.2);
	settings.scale *= scale;
	settings.offset_x = e.offsetX + (settings.offset_x - e.offsetX) * scale;
	settings.offset_y = e.offsetY + (settings.offset_y - e.offsetY) * scale;
	render(dump_data);
};

var update_position = function(e){
	var scale = settings.scale;
	var mouse_x = (e.offsetX - settings.offset_x) / scale;
	var mouse_y = (e.offsetY - settings.offset_y) / -scale;
	$("#mouse-x").text(mouse_x.toFixed(3));
	$("#mouse-y").text(mouse_y.toFixed(3));
};
var mouse_down = function(e){
	update_position(e);
	if(e.button != 0){ return; }
	e.preventDefault();
	mouse_state.dragging = true;
	mouse_state.prev_x = e.screenX;
	mouse_state.prev_y = e.screenY;
};
var mouse_up = function(e){
	update_position(e);
	if(e.button != 0){ return; }
	e.preventDefault();
	mouse_state.dragging = false;
};
var mouse_move = function(e){
	if(!mouse_state.dragging){
		update_position(e);
		return;
	}
	var delta_x = e.screenX - mouse_state.prev_x;
	var delta_y = e.screenY - mouse_state.prev_y;
	settings.offset_x += delta_x;
	settings.offset_y += delta_y;
	mouse_state.prev_x = e.screenX;
	mouse_state.prev_y = e.screenY;
	render(dump_data);
};

var initialize = function(){
	$("#file-selector").submit(function(e){
		e.preventDefault();
		e.stopImmediatePropagation();
		if(document.querySelector("#input-file").files.length == 0){
			return false;
		}
		var file = document.querySelector("#input-file").files[0];
		var reader = new FileReader();
		reader.onload = function(e){
			var data = jQuery.parseJSON(e.target.result);
			dump_data = data;
			make_list(data);
			render(data);
		};
		reader.readAsText(file);
		return false;
	});
	$("#grid-check").change(function(){ render(dump_data); });
	$("#subgrid-check").change(function(){ render(dump_data); });
	$("#layer-show-all").click(function(){
		set_all_visibility(dump_data, true);
	});
	$("#layer-hide-all").click(function(){
		set_all_visibility(dump_data, false);
	});
	var svg = document.querySelector("#svg");
	svg.addEventListener("DOMMouseScroll", wheel);
	svg.addEventListener("mousewheel", wheel);
	svg.addEventListener("mousedown", mouse_down);
	svg.addEventListener("mouseup", mouse_up);
	svg.addEventListener("mousemove", mouse_move);
};

document.addEventListener("DOMContentLoaded", initialize, false);
window.addEventListener("load", initialize, false);
