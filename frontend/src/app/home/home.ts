import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-home',
  imports: [CommonModule],
  templateUrl: './home.html',
  styleUrl: './home.scss'
})
export class Home {
  ssids: string[] = ['Network1', 'HomeWiFi', 'Guest', 'Office', 'Network1', 'HomeWiFi', 'Guest', 'Office', 'Network1', 'HomeWiFi', 'Guest', 'Office'];
  deviceType: { type: string; amount: number }[] = [
    { type: 'Camera', amount: 10 },
    { type: 'TV', amount: 4 },
    { type: 'Phone', amount: 13 },
    { type: 'Computer', amount: 2 },
  ];
}
