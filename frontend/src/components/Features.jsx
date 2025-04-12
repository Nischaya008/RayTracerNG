import { motion } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { FaLightbulb, FaCogs, FaChartLine, FaDesktop, FaCode } from 'react-icons/fa';

const features = [
  {
    icon: <FaLightbulb className="w-8 h-8" />,
    title: 'Real-Time Ray Tracing',
    description: 'Experience stunning light reflections and physics in real-time with configurable ray properties.',
  },
  {
    icon: <FaCogs className="w-8 h-8" />,
    title: 'Interactive Light Physics',
    description: 'Manipulate light sources and observe complex reflection patterns with intuitive controls.',
  },
  {
    icon: <FaCode className="w-8 h-8" />,
    title: 'Configurable Reflections',
    description: 'Customize reflection depth, intensity, and length preservation to achieve desired effects.',
  },
  {
    icon: <FaChartLine className="w-8 h-8" />,
    title: 'Performance Monitoring',
    description: 'Track CPU usage, GPU performance, and frame times with built-in monitoring tools.',
  },
  {
    icon: <FaDesktop className="w-8 h-8" />,
    title: 'High-DPI & Cross-Platform',
    description: 'Enjoy crisp visuals on high-DPI displays with full cross-platform compatibility.',
  },
];

const FeatureCard = ({ icon, title, description, index }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, y: 50 }}
      animate={inView ? { opacity: 1, y: 0 } : { opacity: 0, y: 50 }}
      transition={{ duration: 0.5, delay: index * 0.1 }}
      className="glass p-6 rounded-xl hover:scale-105 transition-transform duration-300"
    >
      <div className="text-accent mb-4">{icon}</div>
      <h3 className="text-xl font-display font-bold mb-2">{title}</h3>
      <p className="text-gray-300">{description}</p>
    </motion.div>
  );
};

const Features = () => {
  return (
    <section id="features" className="section">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Powerful Features
          </h2>
          <p className="text-gray-300 text-lg">
            Discover what makes RayTracerNG a cutting-edge ray tracing engine
          </p>
        </motion.div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
          {features.map((feature, index) => (
            <FeatureCard key={index} {...feature} index={index} />
          ))}
        </div>
      </div>
    </section>
  );
};

export default Features; 