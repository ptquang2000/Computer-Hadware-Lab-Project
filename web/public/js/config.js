const setupList = (datas) => {
	let esps = [];
	if (datas){
		Object.entries(datas).map(data => {
			if (!data[0].includes('id') && !data[0].includes('username'))
				esps.push({...data[1], name: data[0]});
		})
		
		let html = ``;
		esps.forEach(esp =>{
			html += `<li esp="${esp.name}">
							<span class="text">ESP: ${esp.name}</span>
							<div>
							<button type="button" onclick="delFunc('${esp.name}','${datas.username}')">delete all</buton>
							</div>`;

			let days = [];
			Object.entries(esp).map(item => {
				if (!item[0].includes('name'))
					days.push({...item[1], date: item[0]});
			});

			days.forEach(day =>{
				Object.entries(day).map(config => {
					if (!config[0].includes('date')){
						html += `<li config="${day.date}${config[0]}">
									<span class="text">date: ${day.date}</span>
									<div>
									<span class="text">start: ${config[0]} duration: ${config[1]}</span>
									<button type="button" onclick="delFunc1('${day.date}${config[0]}','${esp.name}','${datas.username}')">delete</buton>
									</div>
									</li>`;
					}
				})
			})
		});
		document.querySelector('ul').innerHTML = html;
	}
};

function delFunc1(datetime, esp, username) {
	firebase.database().ref('users/' + username + '/' + esp + '/' + datetime.slice(0, -9) + '/' + datetime.slice(-9)).remove();
}

function delFunc(esp, username) {
	firebase.database().ref('users/' + username + '/' + esp).remove();
}