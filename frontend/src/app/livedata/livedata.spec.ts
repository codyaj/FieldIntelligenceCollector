import { ComponentFixture, TestBed } from '@angular/core/testing';

import { Livedata } from './livedata';

describe('Livedata', () => {
  let component: Livedata;
  let fixture: ComponentFixture<Livedata>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [Livedata]
    })
    .compileComponents();

    fixture = TestBed.createComponent(Livedata);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
