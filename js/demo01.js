// 移动端菜单切换
const mobileMenuBtn = document.getElementById('mobileMenuBtn');
const navLinks = document.getElementById('navLinks');

mobileMenuBtn.addEventListener('click', () => {
    navLinks.classList.toggle('active');
});

// 点击导航链接后关闭移动端菜单
const navItems = document.querySelectorAll('.nav-links a');
navItems.forEach(item => {
    item.addEventListener('click', () => {
        navLinks.classList.remove('active');
    });
});

// 平滑滚动到锚点
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();

        const targetId = this.getAttribute('href');
        if (targetId === '#') return;

        const targetElement = document.querySelector(targetId);
        if (targetElement) {
            window.scrollTo({
                top: targetElement.offsetTop - 80,
                behavior: 'smooth'
            });
        }
    });
});

// 添加滚动效果 - 当滚动到元素时添加动画
window.addEventListener('scroll', () => {
    const featureCards = document.querySelectorAll('.feature-card');
    const advantageItems = document.querySelectorAll('.advantage-item');

    featureCards.forEach(card => {
        const cardTop = card.getBoundingClientRect().top;
        const windowHeight = window.innerHeight;

        if (cardTop < windowHeight - 100) {
            card.style.opacity = '1';
            card.style.transform = 'translateY(0)';
        }
    });

    advantageItems.forEach(item => {
        const itemTop = item.getBoundingClientRect().top;
        const windowHeight = window.innerHeight;

        if (itemTop < windowHeight - 100) {
            item.style.opacity = '1';
            item.style.transform = 'translateY(0)';
        }
    });
});

// 页面加载时初始化一些元素的动画状态
window.addEventListener('load', () => {
    const featureCards = document.querySelectorAll('.feature-card');
    const advantageItems = document.querySelectorAll('.advantage-item');

    featureCards.forEach(card => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(20px)';
        card.style.transition = 'opacity 0.5s, transform 0.5s';
    });

    advantageItems.forEach(item => {
        item.style.opacity = '0';
        item.style.transform = 'translateY(20px)';
        item.style.transition = 'opacity 0.5s, transform 0.5s';
    });

    // 触发一次滚动事件以初始化元素状态
    setTimeout(() => {
        window.dispatchEvent(new Event('scroll'));
    }, 100);
});

// 添加卡片悬停效果增强
document.querySelectorAll('.feature-card, .advantage-item').forEach(card => {
    card.addEventListener('mouseenter', function () {
        this.style.zIndex = '10';
    });

    card.addEventListener('mouseleave', function () {
        this.style.zIndex = '1';
    });
});

// 添加导航栏滚动效果
let lastScrollTop = 0;
window.addEventListener('scroll', () => {
    const header = document.querySelector('header');
    const scrollTop = window.pageYOffset || document.documentElement.scrollTop;

    if (scrollTop > lastScrollTop && scrollTop > 100) {
        // 向下滚动
        header.style.transform = 'translateY(-100%)';
        header.style.transition = 'transform 0.3s';
    } else {
        // 向上滚动
        header.style.transform = 'translateY(0)';
    }

    lastScrollTop = scrollTop;

    // 当滚动到顶部时，移除阴影
    if (scrollTop === 0) {
        header.style.boxShadow = '0 2px 15px rgba(0, 0, 0, 0.3)';
    } else {
        header.style.boxShadow = '0 5px 20px rgba(0, 0, 0, 0.4)';
    }
});

// 悬浮窗按钮点击事件
document.querySelectorAll('.floating-nav-btn').forEach(btn => {
    btn.addEventListener('click', function (e) {
        e.preventDefault(); // 阻止默认行为

        const targetId = this.getAttribute('data-target');
        console.log('点击按钮，目标ID:', targetId); // 调试用

        const targetElement = document.getElementById(targetId);

        if (targetElement) {
            console.log('找到目标元素:', targetElement); // 调试用

            // 平滑滚动到目标位置
            window.scrollTo({
                top: targetElement.offsetTop - 100,
                behavior: 'smooth'
            });

            // 添加果冻动画效果
            this.classList.add('jelly-effect');

            // 移除动画类以便下次点击时可以再次触发
            setTimeout(() => {
                this.classList.remove('jelly-effect');
            }, 500);

            // 为对应的卡片添加高亮效果
            const featureCards = document.querySelectorAll('.feature-card');
            featureCards.forEach(card => {
                card.classList.remove('highlight');
            });

            // 根据目标ID高亮对应的卡片
            setTimeout(() => {
                if (targetId === 'products' || targetId === 'about') {
                    document.querySelectorAll('.feature-card').forEach(card => {
                        card.classList.add('highlight');
                    });
                }

                if (targetId === 'solutions') {
                    document.querySelectorAll('.advantage-item').forEach(item => {
                        item.classList.add('highlight');
                    });
                }
            }, 600);

            // 2秒后移除高亮效果
            setTimeout(() => {
                document.querySelectorAll('.feature-card, .advantage-item').forEach(el => {
                    el.classList.remove('highlight');
                });
            }, 2000);
        } else {
            console.error('未找到目标元素:', targetId); // 调试用
        }
    });
});